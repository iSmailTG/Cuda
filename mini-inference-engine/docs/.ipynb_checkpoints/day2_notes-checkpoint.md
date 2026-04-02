# Day 2 Notes — CUDA Mental Model + Writing the Kernel

## The GPU Mental Model

A GPU is not a faster CPU. It is a different kind of processor built for one purpose: running the same function thousands of times simultaneously, each time on different data.

When you launch a CUDA kernel, the GPU runs your function in parallel across thousands of **threads**. Each thread executes the same code but works on a different piece of the data.

---

## The Thread Hierarchy

Threads are organized in two levels:

**Threads → Blocks → Grid**

- A **thread** is a single execution unit. It runs your kernel function once.
- Threads are grouped into **blocks**. A typical block is 16×16 = 256 threads.
- Blocks are grouped into a **grid**. The grid covers the entire output matrix.

Each thread knows two things about itself:

```
blockIdx  — which block in the grid is this thread in?
threadIdx — which position within that block?
```

From these two numbers, every thread computes its unique output position:

```cuda
int row = blockIdx.y * blockDim.y + threadIdx.y;
int col = blockIdx.x * blockDim.x + threadIdx.x;
```

This is the most important formula in CUDA. Every kernel starts here.

---

## What the Kernel Actually Does

Each thread computes **one element** of the output matrix C.

To compute C[row][col], the thread takes the dot product of:
- Row `row` of matrix A
- Column `col` of matrix B

```cuda
float sum = 0.0f;
for (int i = 0; i < K; i++) {
    sum += A[row * K + i] * B[i * N + col];
}
C[row * N + col] = sum;
```

That loop runs K times per thread. Every thread does this independently, in parallel.

---

## GPU Memory Hierarchy

| Memory Type   | Latency    | Size per block | Who can access it     |
|---------------|------------|----------------|-----------------------|
| Registers     | ~1 cycle   | Very small     | One thread only       |
| Shared memory | ~4 cycles  | 48 KB          | All threads in a block|
| Global memory | ~400 cycles| 16 GB+         | All threads on GPU    |

My naive kernel reads A and B from **global memory** every iteration of the loop. That means 400 cycles of latency per access, K times per thread.

This is why my kernel will be slow compared to PyTorch. PyTorch uses **cuBLAS**, which loads tiles of A and B into shared memory first, so threads share the fast cached copy instead of each hitting global memory independently.

I don't need to fix this yet. But I need to understand why the gap exists.

---

## The Guard Condition

The grid might be larger than the matrix. If the matrix size is not divisible by the block size, some threads in the last block will have `row >= M` or `col >= N`. Those threads must do nothing.

```cuda
if (row < M && col < N) {
    // safe to compute
}
```

Without this guard, those threads would write to memory addresses outside the output array. That is undefined behavior — memory corruption, wrong results, crashes.

---

## What `extern "C"` Does

CUDA is compiled with a C++ compiler. C++ **mangles** function names — it appends type information to the name so overloaded functions can coexist. The function `matmul_cuda` might be stored as `_Z12matmul_cudaPfS_S_iii` internally.

ctypes (which we use on Day 4) looks up functions by their plain name. If the name is mangled, ctypes cannot find it.

`extern "C"` tells the compiler: do not mangle this function name. Keep it as `matmul_cuda`.

```cuda
extern "C" {
    void matmul_cuda(float* A, float* B, float* C, int M, int N, int K) {
        ...
    }
}
```

---

## The Launch Configuration

```cuda
dim3 blockDim(16, 16);               // 256 threads per block
dim3 gridDim((N+15)/16, (M+15)/16);  // enough blocks to cover the output
matmul_kernel<<<gridDim, blockDim>>>(A, B, C, M, N, K);
cudaDeviceSynchronize();
```

- `blockDim(16, 16)` means each block has 16 columns × 16 rows of threads = 256 threads total.
- `gridDim` is calculated with ceiling division so no output element is missed.
- `cudaDeviceSynchronize()` blocks the CPU until all GPU threads have finished. Without it, the CPU would read C before the GPU is done writing it.

---

## Day 2 Checkpoint

- [x] Understand the thread hierarchy: thread → block → grid
- [x] Understand how `blockIdx` and `threadIdx` map to output positions
- [x] Understand why the guard condition is necessary
- [x] Understand why global memory makes the naive kernel slow
- [x] `kernels/matmul.so` compiles with no errors
- [x] Can explain `extern "C"` and why it is needed for ctypes
