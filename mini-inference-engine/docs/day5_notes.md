# Day 5 Notes — cpp_extension Bridge

## What I built today

I rewrote the same CUDA matmul kernel from Day 4 but this time exposed it to Python using `torch.utils.cpp_extension` instead of ctypes. The result is a cleaner, more production-ready bridge: Python calls `matmul_ext.matmul(A, B)` and gets a `torch.Tensor` back, no pointer casting, no manual compilation, no `argtypes` declarations.

Two files were created:
- `kernels/matmul_ext.cu` — the CUDA kernel + C++ bridge function + PYBIND11 registration
- `src/kernel_ext.py` — loads and wraps the extension in a Python function

---

## What is torch.utils.cpp_extension

`torch.utils.cpp_extension.load()` is a function that:
1. Compiles a `.cu` or `.cpp` file with `nvcc` automatically (no manual shell commands)
2. Links the resulting `.so` against PyTorch's C++ libraries
3. Returns a Python module with all the functions registered via `PYBIND11_MODULE`

It caches the compiled result — on the second import it skips recompilation unless the source file changed. This is how production inference libraries like vLLM and nano-vLLM expose their custom CUDA kernels.

```python
matmul_ext = load(
    name    = "matmul_ext",
    sources = ["kernels/matmul_ext.cu"],
    verbose = True,
)
```

---

## What PYBIND11_MODULE does

pybind11 is the library that connects C++ and Python. The macro at the bottom of the `.cu` file registers your C++ functions as a Python-callable module:

```cpp
PYBIND11_MODULE(TORCH_EXTENSION_NAME, m) {
    m.def("matmul", &matmul_cuda_ext, "Matrix multiply (CUDA)");
}
```

- `TORCH_EXTENSION_NAME` is a macro filled in automatically with the `name` you passed to `load()` — in this case `"matmul_ext"`
- `m.def("matmul", ...)` registers the C++ function `matmul_cuda_ext` under the Python name `"matmul"`
- After this, Python can call `matmul_ext.matmul(A, B)` directly

Without `PYBIND11_MODULE`, the compiled `.so` would exist but Python would have no way to find or call the functions inside it.

---

## What TORCH_CHECK does and why it is better than Python assert

```cpp
TORCH_CHECK(A.is_cuda(), "A must be on GPU");
```

`TORCH_CHECK` is the C++ equivalent of Python's `assert`, but integrated into PyTorch's error system. When the condition is false, it raises a clean `RuntimeError` in Python — the same kind you'd get from any standard PyTorch operation — instead of a raw C++ segfault or an `AssertionError`.

Why it's better than `assert`:
- `assert` can be compiled out in optimized builds (`-DNDEBUG`), `TORCH_CHECK` cannot
- The error message propagates cleanly through the Python/C++ boundary
- It produces a proper Python traceback instead of crashing the process

---

## What torch::Tensor gives me in C++ that float* does not

A raw `float*` pointer is just a memory address. It carries no information about shape, dtype, device, or strides. You have to pass all of that separately and trust that nothing went wrong.

`torch::Tensor` is a full object. In C++ you can query it directly:

```cpp
A.size(0)            // rows
A.size(1)            // cols
A.dtype()            // torch::kFloat32
A.is_cuda()          // true if on GPU
A.is_contiguous()    // true if memory is sequential
A.data_ptr<float>()  // raw float* pointer — only needed when launching the kernel
A.options()          // copies device + dtype — used to create output tensor
```

This is why the bridge function needs no `argtypes`, no manual pointer casting, and no separate shape arguments. The tensor carries all of that itself.

---

## ctypes vs cpp_extension — when would you use each?

| | ctypes | cpp_extension |
|---|---|---|
| Compilation | Manual (`nvcc` shell command) | Automatic on first import |
| Type declarations | Manual (`argtypes`, `restype`) | None needed — C++ has the types |
| Pointer handling | Manual (`ctypes.cast`, `data_ptr()`) | Not needed — pass `torch::Tensor` directly |
| Error checking | Python `assert` | `TORCH_CHECK` — clean RuntimeError |
| Return value | You allocate C manually and fill it | Function returns `torch::Tensor` |
| Used in production | Rarely | Always (vLLM, nano-vLLM, Flash Attention) |

**Use ctypes when:** you are learning the mechanics, calling a pre-compiled library you didn't write, or you have a very simple one-off call.

**Use cpp_extension when:** you are writing production code, you want PyTorch to handle compilation and linking, or you want clean error messages and tensor-aware C++ code.

The reason Day 4 used ctypes first: it forces you to see every step that cpp_extension hides. Once you've done it manually, you understand what "automatic" actually means.

---

## What surprised me today

- A single `#include <torch/extension.h>` replaces all of the manual setup from ctypes — the header pulls in `torch::Tensor`, `TORCH_CHECK`, and `PYBIND11_MODULE` in one line.
- `A.options()` is a small but elegant detail: it copies device and dtype from an existing tensor, so `torch::zeros({M, N}, A.options())` automatically creates the output on the correct GPU with the correct dtype, without specifying either explicitly.
- The `TORCH_CHECK` for contiguity is actually important in practice — transposing a tensor in PyTorch does not move memory, it just changes the strides. Passing a transposed tensor to a naive kernel that assumes row-major layout would silently compute the wrong answer without the check.
