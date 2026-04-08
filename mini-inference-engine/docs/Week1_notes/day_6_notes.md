# DAY 6 — Profile + Understand the Gap

**Goal:** Measure everything. Understand why PyTorch is faster.  
**Hours:** 07:00 – 17:00 (~10 hours)

---


---

## The Benchmark Script — `examples/benchmark.py`

```python
import torch, sys
sys.path.append("src")
from kernel_wrapper import matmul_cuda
from kernel_ext    import matmul_cuda_v2


def bench(fn, A, B, n=100, label=""):
    # Warmup: first runs are slow due to JIT compilation
    for _ in range(10): fn(A, B)
    torch.cuda.synchronize()

    # CUDA Events: the correct way to time GPU operations.
    # time.time() measures CPU time, not GPU execution time.
    start = torch.cuda.Event(enable_timing=True)
    end   = torch.cuda.Event(enable_timing=True)
    start.record()
    for _ in range(n): fn(A, B)
    end.record()
    torch.cuda.synchronize()

    ms = start.elapsed_time(end) / n
    print(f"{label:40s} {ms:8.3f} ms")
    return ms


print(f"{'Operation':<40} {'Time':>8}")
print("-" * 52)

for M, K, N in [(128,128,128), (512,512,512), (1024,1024,1024)]:
    A = torch.randn(M, K, device="cuda", dtype=torch.float32)
    B = torch.randn(K, N, device="cuda", dtype=torch.float32)
    print(f"\n=== {M}x{K} @ {K}x{N} ===")
    t1 = bench(matmul_cuda,    A, B, label="  ctypes bridge")
    t2 = bench(matmul_cuda_v2, A, B, label="  cpp_extension bridge")
    t3 = bench(torch.matmul,   A, B, label="  PyTorch (cuBLAS)")
    print(f"  Slowdown vs PyTorch : {t2/t3:.1f}x")
    # FLOP count: matmul (M,K)@(K,N) = 2*M*K*N operations
    flops  = 2 * M * K * N
    gflops = flops / (t3 / 1000) / 1e9
    print(f"  PyTorch GFLOP/s     : {gflops:.1f}")
```

---

## Why is PyTorch (cuBLAS) So Much Faster?

Your naive kernel reads A and B from **global memory** K times per output element.

- Global memory access takes ~400 cycles
- With K=1024, that is `400 × 1024` cycles per element
- This is extremely wasteful

### cuBLAS uses Shared Memory Tiling:

1. Load a small **tile** of A and B from global memory into **shared memory** (fast cache, ~4 cycles)
2. All threads in the block use the **same shared memory copy** — no redundant global reads
3. Move to the next tile. Repeat.

This reduces global memory accesses by a factor equal to the tile size (typically 16–32x).

**Your kernel will be 10–50x slower on large matrices. This is expected.** Knowing WHY is what matters.

### Memory Hierarchy (recap)

| Memory | Speed | Size | Scope |
|---|---|---|---|
| Global memory | ~400 cycles | 16GB+ | All threads |
| Shared memory | ~4 cycles | 48KB/block | Threads in same block |
| Registers | ~1 cycle | Very limited | One thread |

---

## FLOP Counting — A Skill You Will Use Forever

- **FLOP** = Floating Point Operation. One multiply or one add.
- A matmul of shape `(M,K) @ (K,N)` requires approximately **`2 × M × K × N` FLOPs**
- A T4 GPU can do ~65 TFLOP/s (float32 theoretical peak)
- When PyTorch achieves 40 GFLOP/s → it's using ~60% of the GPU
- When your kernel achieves 1 GFLOP/s → it's using ~0.0015% of the GPU

This number — **FLOP/s utilization** — is how kernel engineers measure optimization progress.

---

## Why CUDA Events Instead of `time.time()`?

GPU operations are **asynchronous**. When the CPU launches a kernel, it immediately moves on to the next line without waiting for the GPU to finish. 

- `time.time()` only measures CPU time — it returns almost immediately
- `torch.cuda.synchronize()` forces the CPU to wait for GPU completion
- **CUDA Events** are timestamps recorded directly on the GPU — they are accurate

Always use CUDA Events for GPU benchmarking. Never use `time.time()`.

---

## The 3 Benchmark Patterns to Memorize

```python
# 1. Always warmup
for _ in range(10): fn(A, B)

# 2. Always use CUDA Events
start = torch.cuda.Event(enable_timing=True)
start.record()
# ... your work ...
end.record()
torch.cuda.synchronize()
ms = start.elapsed_time(end) / n_runs

# 3. FLOP count for matmul (M,K) @ (K,N)
flops = 2 * M * K * N
```

---

