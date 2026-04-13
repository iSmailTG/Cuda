
import torch
import ctypes
import os
import subprocess

if not os.path.exists('./kernels/matmul.so'):
  subprocess.run(['nvcc', '-shared', '-o', './kernels/matmul.so', './kernels/matmul.cu', '-Xcompiler', '-fPIC', '-O2'], check=True)

matmul_lib = ctypes.CDLL('./kernels/matmul.so')
matmul_lib.matmul_cuda.argtypes = [
    ctypes.POINTER(ctypes.c_float),
    ctypes.POINTER(ctypes.c_float),
    ctypes.POINTER(ctypes.c_float),
    ctypes.c_int,
    ctypes.c_int,
    ctypes.c_int,
]

matmul_lib.matmul_cuda.restype = None


def matmul_cuda(A: torch.Tensor, B: torch.Tensor) -> torch.Tensor:
  assert A.is_cuda and B.is_cuda, "must be on device"
  assert A.dtype == torch.float32, "support float32 only"
  assert A.is_contiguous(), "must be contiguous on memory"
  assert B.is_contiguous(), "must be contiguous"

  M, K = A.shape
  K2, N = B.shape
  assert K == K2, f"mismatch of shapes: ({M}, {K} X {K}, {N})"
  C = torch.zeros(M, N, device = 'cuda', dtype = torch.float32)

  A_ptr = ctypes.cast(A.data_ptr(), ctypes.POINTER(ctypes.c_float))
  B_ptr = ctypes.cast(B.data_ptr(), ctypes.POINTER(ctypes.c_float))
  C_ptr = ctypes.cast(C.data_ptr(), ctypes.POINTER(ctypes.c_float))

  matmul_lib.matmul_cuda(A_ptr, B_ptr, C_ptr, M, N, K)
  return C
