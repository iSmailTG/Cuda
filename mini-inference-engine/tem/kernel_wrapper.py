
import torch
import os
import ctypes
import subprocess

if not os.path.exists('./tem/matmul_from_memory.so'):
  subprocess.run(['nvvc', '-shared', '-o', './tem/matmul_from_memory.so', './tem/matmul_from_memory.cu', '-Xcompiler', '-fPIC', '-O2'])

matmul_lib = ctypes.CDLL('./tem/matmul_from_memory.so')
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
  assert A.is_coniguous(), "must be contiguous"
  assert B.is_contiguous(), "must be contiguous"

  M, K = A.shape
  K2, N = B.shape
  assert K == K2, "A cols must match B row is shape"

  C = torch.zeros(M, N, device='cuda', dtype= torch.float32)
  
  A_ptr = ctypes.cast(A.data_ptr(), ctypes.POINTER(ctypes.c_float))
  B_ptr = ctypes.cast(B.data_ptr(), ctypes.POINTER(ctypes.c_float))
  C_ptr = ctypes.cast(C.data_ptr(), ctypes.POINTER(ctypes.c_float))

  matmul_lib.matmul_cuda(A_ptr, B_ptr, C_ptr, M, N, K)
  return C
  
  
  matmul_lib.matmul_cuda
