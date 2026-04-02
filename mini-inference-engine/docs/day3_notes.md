# Day 3 Notes — Memorize the Kernel + Deep Dive

## The Point of Day 3

Day 3 has no new code. The goal is to own yesterday's code permanently.

---

## Row-Major Indexing — The Most Important Concept Today

CUDA does not have 2D arrays, a matrix in GPU memory is stored as a single flat 1D array, row by row. This is called **row-major layout**.

For a matrix A of shape (M, K):

```
Element at row r, column c  →  stored at index: r * K + c
```

Example — matrix A of shape (3, 3):

```
A = [[1, 2, 3],
     [4, 5, 6],
     [7, 8, 9]]

Flat in memory: [1, 2, 3, 4, 5, 6, 7, 8, 9]
                  0  1  2  3  4  5  6  7  8

A[1][2] = 6  →  index = 1 * 3 + 2 = 5  →  memory[5] = 6  ✓
```

So in the kernel:
- `A[row * K + i]` = element at row `row`, column `i` of matrix A
- `B[i * N + col]` = element at row `i`, column `col` of matrix B
- `C[row * N + col]` = element at row `row`, column `col` of matrix C

If we ever swap `K` and `N` in the wrong place, we will read the wrong memory address and get silently wrong results, just garbage.

---

## Ceiling Division — Why `(N + 15) / 16`

When launching a grid, you need enough blocks to cover every column of the output.

- If N = 32: `32 / 16 = 2` blocks. Exact. Fine.
- If N = 33: we need 3 blocks. But `33 / 16 = 2` in integer division. **One column is missed.**
- Fix: `(33 + 15) / 16 = 48 / 16 = 3`. Correct.

The general formula for ceiling division:

```
ceil(N / B) = (N + B - 1) / B
```

With block size 16: `(N + 15) / 16`

This is why the guard condition exists. The extra threads in the last block have `col >= N`, and the guard stops them from writing out of bounds.

---

## What breaks if we change some elements of the code:

These experiments build intuition faster than reading ever will.

**Experiment 1 — Change blockDim to (32, 32):**
It still compiles. 32×32 = 1024 threads per block, which is the hardware maximum on most GPUs. If we go higher (e.g. 33×33), the kernel launch fails at runtime —> not a compile error.

**Experiment 2 — Remove `cudaDeviceSynchronize()`:**
The CPU reads the result before the GPU finishes writing it. Tests may pass sometimes and fail other times depending on timing. This is a race condition. It is hard to debug because it is non-deterministic.

**Experiment 3 — Change `A[row * K + i]` to `A[row + K * i]`:**
We are now reading column-major instead of row-major, the kernel computes the wrong dot product. It compiles and runs without errors : it just produces garbage numbers: worst kind of bug.

**Experiment 4 — Remove the guard condition:**
Threads in the last block that are out of bounds write to arbitrary GPU memory addresses. This causes memory corruption. The result may be wrong or the program may crash. On small matrices that divide evenly by 16, you won't see it , which makes it even more dangerous.

---

## Where Matmul Appears in Llama

Every attention operation in Llama is a matrix multiply. This kernel is not a toy — it is the fundamental operation of the entire model.

In one attention head:

```python
Q = x @ W_q          # matmul — project input to queries
K = x @ W_k          # matmul — project input to keys
V = x @ W_v          # matmul — project input to values

scores = Q @ K.T / sqrt(head_dim)   # matmul — attention scores
                                     # grows quadratically with sequence length

output = scores @ V  # matmul — weighted sum of values
```

Then after attention, the feed-forward block:

```python
gate   = x @ W_gate  # matmul
up     = x @ W_up    # matmul
x      = (gate * silu(up)) @ W_down  # matmul
```

Every single one of those is a matrix multiply. The kernel written on Day 2 is the building block for all of week 2.

---

