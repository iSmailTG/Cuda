
import torch
import os
import subprocess
import ctypes

if not os.path.exists('./tem/matmul_cuda.so'):
  subprocess.run(['nvcc', '-shared', '-o', './tem/matmul_cuda.so', './tem/matmul_cuda.cu', '-Xcompiler', '-fPIC', '-O2'])

matmul_lib = ctypes.CDLL('./tem/matmul_cuda.so')
matmul_lib.matmul_cuda.argtypes = [
    ctypes.POINTER(ctypes.c_float),
    ctypes.POINTER(ctypes.c_float),
    ctypes.POINTER(ctypes.c_float),
    ctypes.c_int,
    ctypes.c_int,
    ctypes.c_int
]
matmul_lib.matmul_cuda.restype = None

def matmul_cuda(A: torch.Tensor, B: torch.Tensor) -> torch.Tensor:
  assert A.is_cuda and B.is_cuda, "must be on GPU"
  assert A.dtype == torch.float32, "support float32 only"
  assert A.is_contiguous(), "Must be contiguous"
  assert B.is_contiguous(), "Must be contiguous"

  M, K  = A.shape
  K2, N = B.shape
  assert K == K2, "A col must match B row"
  C = torch.zeros(M, N, device='cuda', dtype = torch.float32)

  A_ptr = ctypes.cast(A.data_ptr(), ctypes.POINTER(ctypes.c_float))
  B_ptr = ctypes.cast(B.data_ptr(), ctypes.POINTER(ctypes.c_float))
  C_ptr = ctypes.cast(C.data_ptr(), ctypes.POINTER(ctypes.c_float))

  matmul_lib.matmul_cuda(A_ptr, B_ptr, C_ptr, M, N, K)
  return C
