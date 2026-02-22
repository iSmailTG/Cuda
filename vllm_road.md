Good. Let's go deep on each.

---

## 1. Why KV Cache is the Bottleneck

### The Math

**Transformer inference has two phases:**

**Prefill (prompt processing):**
- Input: entire prompt at once
- Compute: full attention matrix for all prompt tokens
- Output: KV cache for prompt + first generated token

**Decode (autoregressive generation):**
- Input: one new token
- Compute: attention between new token and ALL previous tokens
- Output: append to KV cache

**Memory requirement per token:**
```
KV_memory_per_token = 2 × n_layers × d_model × 2_bytes (FP16)
                    = 2 × n_layers × d_model × 2

Example (Llama-13B):
- n_layers = 40
- d_model = 5120
- Per token: 2 × 40 × 5120 × 2 = 819KB per token

For sequence_length = 2048:
- KV cache = 819KB × 2048 ≈ 1.6GB per request
```

**The bottleneck emerges:**
```
GPU memory budget (A100 40GB):
- Model weights: ~13GB (FP16)
- Activations: ~2-3GB
- Available for KV: ~25GB

Traditional static allocation:
- Allocate max_seq_len upfront: 1.6GB × batch_size
- Max batch: 25GB / 1.6GB ≈ 15 requests

But most requests don't use full 2048 tokens!
- Average request: 512 tokens
- Wasted memory: (2048 - 512) × 819KB = 1.2GB per request
- Actual utilization: 25%
```

**Why it's worse than weight memory:**
- Weights: static, loaded once
- KV cache: dynamic, grows during generation, varies per request
- Can't offload to CPU (would destroy throughput)

### During Decode Phase

**Memory bandwidth bottleneck:**
```
Decode iteration:
- Read entire KV cache (all previous tokens)
- Compute attention (mostly memory-bound)
- Write new KV pair

For sequence_length = 1024:
- Read: 1024 × 819KB ≈ 820MB
- Compute: small (just one token's attention)
- Memory bandwidth becomes the limit
```

This is why decode is **memory-bound** while prefill is **compute-bound**.

---

## 2. How PagedAttention Works

### Core Concept: Indirection

**Traditional attention:**
```python
# KV cache stored contiguously
K = [k₀, k₁, k₂, k₃, ..., k_n]  # shape: [seq_len, d_model]
V = [v₀, v₁, v₂, v₃, ..., v_n]

# Attention: Q @ K^T
attention = softmax(Q @ K.T) @ V
```

**PagedAttention:**
```python
# KV cache split into blocks
blocks = {
    block_0: [k₀, k₁, ..., k₁₅],  # 16 tokens per block
    block_1: [k₁₆, k₁₇, ..., k₃₁],
    block_2: [k₃₂, k₃₃, ..., k₄₇],
    ...
}

# Block table (indirection)
block_table = [ptr_to_block_0, ptr_to_block_1, ptr_to_block_2, ...]

# Attention kernel uses block_table to find KV data
```

### The Mechanism

**Block structure:**
```
Block size: 16 tokens (configurable)
Each block stores: K and V for those 16 tokens across all layers

Physical memory layout:
[Block 0: layer0_K, layer0_V, layer1_K, layer1_V, ...]
[Block 1: layer0_K, layer0_V, layer1_K, layer1_V, ...]
[Block 2: ...]
```

**Block table per request:**
```
Request with 50 tokens needs 4 blocks:
block_table = [7, 3, 15, 22]  # Physical block IDs

Logical token 0-15   → Physical block 7
Logical token 16-31  → Physical block 3
Logical token 32-47  → Physical block 15
Logical token 48-49  → Physical block 22 (partially filled)
```

**Attention computation with indirection:**
```cuda
// Simplified CUDA kernel logic
for (int token_idx = 0; token_idx < seq_len; token_idx++) {
    int block_idx = token_idx / BLOCK_SIZE;          // Which block?
    int offset = token_idx % BLOCK_SIZE;             // Offset within block
    int physical_block = block_table[block_idx];     // Lookup physical block
    
    // Load K, V from physical block
    K = blocks[physical_block][offset];
    V = blocks[physical_block][offset];
    
    // Compute attention...
}
```

### Why This Wins

**1. No pre-allocation waste:**
```
Traditional: Allocate 2048 tokens upfront → Use 512 → Waste 75%
PagedAttention: Allocate 32 blocks (512 tokens) → Use 32 → Waste 0%
```

**2. Dynamic growth:**
```
Token 0-15:   Allocate block 0
Token 16-31:  Allocate block 1
Token 32-47:  Allocate block 2
...
No need to know final length upfront
```

**3. Prefix sharing (massive win):**
```
Request 1: [System prompt: 200 tokens] + [User query A: 50 tokens]
Request 2: [System prompt: 200 tokens] + [User query B: 50 tokens]

Traditional: Store system prompt KV twice → 400 tokens stored
PagedAttention: Share first 13 blocks → 213 tokens stored (35% savings)

block_table_1 = [shared_0, ..., shared_12, unique_13, unique_14, unique_15]
block_table_2 = [shared_0, ..., shared_12, unique_16, unique_17, unique_18]
```

**4. Flexible swapping:**
```
Low-priority request running out of GPU memory?
- Swap last few blocks to CPU
- Continue with reduced context
- Swap back when memory available

Can't do this with contiguous allocation
```

### The Cost

**Indirection overhead:**
- Extra memory reads through block_table
- Slightly more complex kernel logic
- ~5-10% compute overhead

**But memory savings → more batching → net win**

---

## 3. Continuous Batching vs Static Batching

### Static Batching (Traditional)

**How it works:**
```
Collect B requests
Pad all to max_length
Process entire batch together
Wait for slowest request to finish
Return all results
Start next batch
```

**Example timeline:**
```
Batch of 4 requests:
Req A: 100 tokens | ████              | Finish at iteration 100
Req B: 500 tokens | ████████████████  | Finish at iteration 500
Req C: 200 tokens | ████████          | Finish at iteration 200
Req D: 150 tokens | ██████            | Finish at iteration 150

GPU utilization:
Iter 0-100:   [A, B, C, D] - 4 requests (100% batch)
Iter 101-150: [_, B, C, D] - 3 requests (75% batch)
Iter 151-200: [_, B, C, _] - 2 requests (50% batch)
Iter 201-500: [_, B, _, _] - 1 request  (25% batch)

Average utilization: 62.5%
Req A waits 400 extra iterations doing nothing
```

### Continuous Batching (Iteration-Level)

**How it works:**
```
Maintain running batch
Each iteration:
  - Remove finished requests
  - Add new requests from queue (up to max batch size)
  - Process current batch
```

**Same example with continuous batching:**
```
Iter 0-100:   [A, B, C, D] - 4 requests
Iter 100:     A finishes → Add E immediately
Iter 101-150: [E, B, C, D] - 4 requests
Iter 150:     D finishes → Add F immediately
Iter 151-200: [E, B, C, F] - 4 requests
Iter 200:     C finishes → Add G immediately
Iter 201-500: [E, B, F, G] - 4 requests

Average utilization: 100% (always at max batch size)
Req A returns immediately at iteration 100
```

### The Key Difference

**Static: Request-level batching**
- Batch = unit of work
- All requests start and end together
- Synchronization barrier at batch boundaries

**Continuous: Iteration-level batching**
- Iteration = unit of work
- Requests join/leave independently
- No synchronization barriers

### Implementation Challenge

**Problem: Requests are at different positions**

```
Iteration 150 state:
Req E: position 50  (just started)
Req B: position 150 (halfway done)
Req C: position 150
Req D: position 150

How do you batch these in one forward pass?
```

**Solution: Split prefill and decode**

```python
# Prefill: Process new prompts (long sequences)
prefill_batch = [requests needing initial KV cache]
run_prefill(prefill_batch)  # Separate kernel optimized for long sequences

# Decode: Process ongoing generation (single tokens)
decode_batch = [requests generating next token]
run_decode(decode_batch)    # Separate kernel optimized for single tokens

# Can run both in same iteration if memory permits
```

**Why this works:**
- Prefill: High compute intensity, different kernel characteristics
- Decode: Memory-bound, benefits from batching
- Separate = better kernel optimization for each

### Scheduling Complexity

**Static batching scheduler:**
```python
while True:
    batch = wait_for_B_requests()
    results = process_batch(batch)
    return_results(results)
```

**Continuous batching scheduler:**
```python
running = []
waiting = Queue()

while True:
    # Remove finished
    finished = [r for r in running if r.done()]
    running = [r for r in running if not r.done()]
    
    # Add new (up to max batch size)
    while len(running) < MAX_BATCH and not waiting.empty():
        running.append(waiting.pop())
    
    # Run iteration
    run_iteration(running)
```

Plus: preemption, swapping, priority, memory management...

---

## 4. Scheduling Tradeoffs

### The Core Tension

**Throughput vs Latency:**
- Throughput: requests/second (batch size matters)
- Latency: time per request (queuing time matters)

**You can't optimize both simultaneously.**

### Scheduling Policies

**1. First-Come-First-Serve (FCFS)**
```
Requests processed in arrival order
+ Fair
+ Simple
- Head-of-line blocking (long request blocks short ones)
```

**2. Shortest-Job-First (SJF)**
```
Prioritize requests with fewer remaining tokens
+ Lower average latency
- Starvation for long requests
- Need to predict length (unreliable)
```

**3. Preemptive Priority**
```
High-priority requests can preempt low-priority
+ Latency guarantees for important requests
- Overhead of swapping KV cache
- Complexity
```

### Preemption Mechanics

**When to preempt:**
```
GPU memory full
New high-priority request arrives
Can't fit in current batch

Decision: Which running request to pause?
- Lowest priority
- Farthest from completion
- Largest KV cache (frees most memory)
```

**How to preempt:**
```python
# 1. Choose victim request
victim = select_victim(running_requests)

# 2. Copy KV cache blocks to CPU
for block in victim.block_table:
    cpu_memory[block] = gpu_memory[block]

# 3. Free GPU blocks
free_gpu_blocks(victim.block_table)

# 4. Remove from running batch
running.remove(victim)
swapped.append(victim)

# 5. Add new request
running.append(new_high_priority_request)
```

**Cost of preemption:**
- PCIe transfer: ~12 GB/s (slow!)
- For 1GB KV cache: ~80ms overhead
- Only worth it if new request is much higher priority

### Memory Pressure Decisions

**Scenario: Running out of GPU memory**

**Option 1: Block new requests**
```
+ No overhead
- Throughput drops
- Latency for waiting requests increases
```

**Option 2: Preempt + swap**
```
+ Can accept new requests
- Swap overhead
- Complexity
```

**Option 3: Reduce batch size**
```
+ Simple
- Lower throughput
- Some requests wait longer
```

**vLLM approach: Hybrid**
```python
if memory_usage > 90%:
    if high_priority_request_waiting:
        preempt_lowest_priority()
        swap_to_cpu()
    else:
        block_new_requests()
        wait_for_completion()
```

### Chunked Prefill Tradeoff

**Problem: Long prompts hog GPU**
```
10k token prompt takes 200ms to prefill
During this, decode requests wait
Decode throughput tanks
```

**Solution: Chunk prefill**
```
Split 10k tokens into chunks of 512
Process 512 tokens, then run decode batch, then next 512, ...

+ Decode latency stays low
- Prefill takes longer (less efficient)
- More scheduler complexity
```

**Tuning parameter:**
```python
max_prefill_tokens_per_iteration = 512  # Small = low decode latency
                                        # Large = high prefill throughput
```

### Real-World Example

**High-throughput scenario (batch service):**
```
- Large batch size (128+)
- Accept high latency (seconds OK)
- Minimize preemption (overhead not worth it)
- Focus: requests/second
```

**Low-latency scenario (interactive chatbot):**
```
- Small batch size (8-32)
- Tight latency SLO (100ms)
- Preempt aggressively for new requests
- Chunked prefill
- Focus: p99 latency
```

---

## 5. Attention Kernel Optimizations

### The Attention Operation

```python
# Naive implementation
Q @ K.T → scores [batch, n_heads, seq_len, seq_len]  # O(n²) memory!
softmax(scores / sqrt(d_k))
scores @ V → output
```

**Problems:**
1. **Memory: O(n²)** - 4k context = 16M elements per head
2. **Memory bandwidth:** Repeatedly reading Q, K, V from HBM
3. **Unfused operations:** Multiple kernel launches

### Memory Hierarchy

**GPU memory speeds (A100):**
```
Registers:      ~20 TB/s   (tiny capacity, per-thread)
Shared memory:  ~15 TB/s   (small, per-SM, explicitly managed)
L2 cache:       ~3 TB/s    (automatic)
HBM (global):   ~1.5 TB/s  (large, main memory)
```

**Naive attention is HBM-bound:**
```
Read Q from HBM → Compute → Write to HBM
Read K from HBM → Compute → Write to HBM
Read scores → Softmax → Write to HBM
Read scores → Read V → Compute → Write output

Total HBM reads/writes: 4x the actual computation needs
```

### FlashAttention: The Breakthrough

**Core idea: Tiling + recomputation + kernel fusion**

**Standard approach:**
```
1. Compute entire attention matrix (materialize in HBM)
2. Softmax
3. Multiply with V
```

**FlashAttention approach:**
```
1. Divide Q, K, V into blocks (tiles)
2. Load one tile of Q, K into shared memory
3. Compute partial attention, keep running statistics for softmax
4. Discard scores, load next tile
5. Online softmax correction
6. Never materialize full attention matrix
```

**Memory complexity:**
```
Standard: O(n²)
FlashAttention: O(n) - only store output and softmax statistics
```

### Online Softmax Trick

**Problem: Softmax needs global max**
```python
# Can't compute softmax on tiles independently
softmax(x) = exp(x) / sum(exp(x))
Need to know max(x) across all tiles
```

**Solution: Online softmax algorithm**
```python
# Process tiles sequentially, maintain running statistics
m = -inf  # running max
d = 0     # running sum
o = 0     # running output

for tile in tiles:
    m_new = max(m, max(tile))
    d = d * exp(m - m_new) + sum(exp(tile - m_new))
    o = o * exp(m - m_new) + output_from_tile
    m = m_new

# Final output
output = o / d
```

This lets you compute softmax one tile at a time without materializing the full matrix.

### Tiling Strategy

**Block dimensions:**
```
Q blocks: [Br, d]   where Br = 128 tokens
K, V blocks: [Bc, d]   where Bc = 64 tokens

Shared memory layout:
Qi: 128 × 64 = 8KB (fp16)
Ki: 64 × 64 = 4KB
Vi: 64 × 64 = 4KB
Total: ~16KB per block (fits comfortably in 48KB shared mem)
```

**Outer loop over Q blocks:**
```python
for q_block in Q_blocks:
    load_to_shared(q_block)  # Load Qi once
    
    for kv_block in KV_blocks:
        load_to_shared(k_block, v_block)  # Load Ki, Vi
        
        # Compute attention for this tile in shared memory
        scores = Qi @ Ki.T
        softmax_partial(scores)  # Online softmax
        output += scores @ Vi
        
        # Discard scores, they're not needed anymore
```

**Why this is fast:**
- Each Q block loaded once, reused across all KV blocks
- All computation happens in shared memory
- HBM accesses minimized

### Memory Access Pattern

**Traditional attention:**
```
HBM reads: Q (n²), K (n²), V (n²)
HBM writes: Scores (n²), Output (n)
Total: O(n²) HBM traffic
```

**FlashAttention:**
```
HBM reads: Q (n), K (n), V (n)  - each read once into shared mem
HBM writes: Output (n)
Total: O(n) HBM traffic
```

**Speedup: 4-8x** (primarily from memory bandwidth reduction)

### PagedAttention Modifications

**Additional complexity: KV cache in blocks**

```cuda
// FlashAttention: contiguous K, V
for (int i = 0; i < seq_len; i++) {
    K[i] = kv_cache[i];
}

// PagedAttention: indirect through block table
for (int i = 0; i < seq_len; i++) {
    int block_idx = i / BLOCK_SIZE;
    int offset = i % BLOCK_SIZE;
    int physical_block = block_table[block_idx];
    K[i] = kv_cache[physical_block][offset];
}
```

**Cost: Extra pointer indirection**
- Cache misses if blocks scattered in memory
- ~5-10% overhead vs contiguous

**Mitigation:**
- Prefetch blocks into shared memory
- Batch multiple block lookups
- Optimize block allocation to maintain locality

### Kernel Fusion Opportunities

**Typical LLM layer:**
```
Input → Attention → Add & Norm → FFN → Add & Norm → Output
```

**Fusion opportunities:**
1. **Attention + Add + RMSNorm:**
```cuda
__global__ void fused_attention_norm(Q, K, V, input, output) {
    attention = compute_attention(Q, K, V);
    residual = attention + input;  // Residual connection
    output = rms_norm(residual);   // Normalize
}
// Single kernel launch instead of 3
```

2. **Quantization + GEMM:**
```cuda
__global__ void quantized_gemm(int8_weights, scales, input, output) {
    weight_fp16 = dequantize(int8_weights, scales);
    output = matmul(input, weight_fp16);
}
// Dequantize on-the-fly, don't store FP16 weights
```

3. **Softmax + Scale:**
```cuda
// Instead of: softmax(scores / sqrt(d_k))
__global__ void scaled_softmax(scores, d_k, output) {
    // Fuse division into softmax computation
}
```

**Benefits:**
- Fewer kernel launches (overhead reduction)
- Intermediate results stay in registers/shared memory
- Reduced HBM traffic

### Practical Optimization Workflow

```
1. Profile with nsys:
   - Identify hotspot kernels (usually attention, GEMM)

2. Drill down with ncu:
   - Memory throughput: Achieved vs theoretical
   - Compute throughput: Achieved vs theoretical
   - Occupancy: How many warps active?
   - Identify: compute-bound or memory-bound?

3. Optimize based on bottleneck:
   
   If memory-bound:
   - Improve memory access patterns (coalescing)
   - Use shared memory / registers more
   - Fuse operations to reduce HBM traffic
   
   If compute-bound:
   - Increase occupancy (more warps)
   - Use Tensor Cores (FP16/BF16/INT8 GEMM)
   - Reduce register usage to fit more warps

4. Iterate and measure
```

---

## What This Means for You

**To work on vLLM-level systems, you need to:**

1. **Trace memory flow** - Where does data live at each step? How much HBM traffic?
2. **Understand scheduling impact** - How does batching policy affect memory? Latency?
3. **Read profiler output** - "80% memory bound" → what optimization applies?
4. **Know the tradeoffs** - Chunked prefill? Preemption? Each has costs.
5. **Think in tiles** - Modern kernels don't process entire tensors at once.

**Your learning path:**
- Implement naive attention, measure HBM traffic
- Implement tiled attention, compare
- Profile both with `ncu`
- Understand why tiled is faster from first principles

This is the depth you need. Go build something.
