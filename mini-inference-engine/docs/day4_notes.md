# Day 4 — ctypes Bridge
## Python Calling Your CUDA Kernel — The Manual Way

**Goal:** Build `src/kernel_wrapper.py` using ctypes to understand what happens underneath  


---

## The Big Idea

My kernel is compiled into a `.so` file — a shared library of compiled C code. ctypes is Python's built-in way to load and call compiled C functions **directly**, without any framework magic in between.


---


## The ctypes Mental Model

Three things we must do **manually** with ctypes:

1. **Load the `.so` file** — `ctypes.CDLL("./kernels/matmul.so")`
2. **Declare argument types** — tell Python what C types the function expects
3. **Get raw memory pointers** — `tensor.data_ptr()` gives the GPU memory address as an integer

Why is this painful? Because Python knows nothing about your C types.
It has no way to know that `matmul_cuda` expects `float*` pointers and `int`s.
We need to declare this explicitly using `.argtypes`.

---

## The Full ctypes Bridge — `src/kernel_wrapper.py`

```python
import ctypes
import torch

# Step 1: Load the compiled shared library
matmul_lib = ctypes.CDLL("./kernels/matmul.so")

# Step 2: Tell Python what types the C function expects.
# Without this, Python passes the wrong binary representation.
matmul_lib.matmul_cuda.argtypes = [
    ctypes.POINTER(ctypes.c_float),  # float* A
    ctypes.POINTER(ctypes.c_float),  # float* B
    ctypes.POINTER(ctypes.c_float),  # float* C
    ctypes.c_int,                    # int M
    ctypes.c_int,                    # int N
    ctypes.c_int,                    # int K
]
matmul_lib.matmul_cuda.restype = None  # void return


def matmul_cuda(A: torch.Tensor, B: torch.Tensor) -> torch.Tensor:
    # Guards: fail loudly rather than produce wrong results silently
    assert A.is_cuda and B.is_cuda,    "Tensors must be on GPU"
    assert A.dtype == torch.float32,   "Only float32 supported"
    assert A.is_contiguous(),          "A must be contiguous"
    assert B.is_contiguous(),          "B must be contiguous"

    M, K  = A.shape
    K2, N = B.shape
    assert K == K2, f"Shape mismatch: ({M},{K}) x ({K2},{N})"

    C = torch.zeros(M, N, device="cuda", dtype=torch.float32)

    # Step 3: Get raw GPU memory addresses and cast them to C pointers
    # .data_ptr() returns the integer GPU memory address of the tensor.
    # ctypes.cast converts it to a float* pointer the C function can use.
    A_ptr = ctypes.cast(A.data_ptr(), ctypes.POINTER(ctypes.c_float))
    B_ptr = ctypes.cast(B.data_ptr(), ctypes.POINTER(ctypes.c_float))
    C_ptr = ctypes.cast(C.data_ptr(), ctypes.POINTER(ctypes.c_float))

    matmul_lib.matmul_cuda(A_ptr, B_ptr, C_ptr, M, N, K)
    return C
```

---

## What `.data_ptr()` Returns

`tensor.data_ptr()` returns an **integer** — the raw GPU memory address where the tensor's data begins.

Think of it like this:
- The tensor is a Python object that **points to** some memory on the GPU
- `.data_ptr()` gives you that raw address as a number (e.g. `139823456`)
- `ctypes.cast(...)` converts that integer into a typed C pointer (`float*`) the C function can use

This is the same address that the GPU hardware will use to read the data. No copies, no intermediate buffers — direct memory access.

---

## Why `is_contiguous()` Matters

PyTorch tensors are **not always stored sequentially in memory**.

Operations like `.transpose()` return a tensor that shares memory with the original but is accessed in a different stride pattern — it is **non-contiguous**.

The kernel assumes **row-major sequential memory**.
If A is non-contiguous, `A[row * K + i]` reads the **wrong address**. The results will be silently wrong with garbage numbers.

```python
A = torch.randn(4, 8, device='cuda')
A_T = A.T   # non-contiguous — shares memory, different stride

print(A.is_contiguous())    # True
print(A_T.is_contiguous())  # False

A_T_fixed = A_T.contiguous()  # forces a fresh copy into sequential memory
print(A_T_fixed.is_contiguous())  # True
```

**The fix:** always call `.contiguous()` before passing a tensor to your kernel if you are not sure.

---

## The 5 ctypes Patterns You Must Know

```python
# 1. Loading the library
lib = ctypes.CDLL("./kernels/matmul.so")

# 2. Declaring argument types
lib.func.argtypes = [ctypes.POINTER(ctypes.c_float), ctypes.c_int, ...]

# 3. Getting a pointer from a tensor
ptr = ctypes.cast(tensor.data_ptr(), ctypes.POINTER(ctypes.c_float))

# 4. The contiguous guard
assert tensor.is_contiguous(), "must be contiguous"

# 5. Calling the function
lib.matmul_cuda(A_ptr, B_ptr, C_ptr, M, N, K)
```

---

## The Test Suite — `tests/test_matmul.py`

```python
import sys, torch
sys.path.append("src")
from kernel_wrapper import matmul_cuda

def test_correctness():
    A = torch.randn(64, 128, device="cuda", dtype=torch.float32)
    B = torch.randn(128, 64, device="cuda", dtype=torch.float32)
    C_yours   = matmul_cuda(A, B)
    C_pytorch = torch.matmul(A, B)
    diff = (C_yours - C_pytorch).abs().max().item()
    print(f"Max diff: {diff:.6f}")
    assert diff < 1e-3, f"Too large: {diff}"
    print("PASS — correctness")

def test_shapes():
    for M, K, N in [(32,64,16), (128,256,64), (512,512,512)]:
        A = torch.randn(M, K, device="cuda", dtype=torch.float32)
        B = torch.randn(K, N, device="cuda", dtype=torch.float32)
        C = matmul_cuda(A, B)
        assert C.shape == (M, N)
        print(f"PASS — shape ({M},{K}) x ({K},{N})")

if __name__ == "__main__":
    test_correctness()
    test_shapes()
    print("All tests passed.")
```

**Why `diff < 1e-3`?** Floating point arithmetic on GPUs is not bit-for-bit identical to CPU. Small differences in operation order produce slightly different results. 1e-3 is a safe tolerance for float32.

---
