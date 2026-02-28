import torch
import ctypes
import subprocess
import os

if not os.path.exists('./kernels/matmul.so'):
  subprocess.run(['nvcc', '-shared', '-o', './kernels/matmul.so', './kernels/matmul.cu', '-Xcompiler',
                  '-fPIC', '-O2'], check=True)

lib_matmul = ctypes.CDLL('./matmul.so')

lib_matmul.args = [ctypes.POINTER(ctypes.c_float),
                   ctypes.POINTER(ctypes.c_float),
                   ctypes.POINTER(ctypes.c_float),
                   ctypes.c_int, ctypes.c_int, ctypes.c_int]
lib_matmul.restype = None

def matmul_cuda(A: torch.Tensor, B: torch.Tensor) -> torch.Tensor:
  assert A.is_cuda and B.is_cuda
  assert A.shape == torch.float32
  assert A.is_contiguous()
  assert B.is_contiguous()

  M, K = A.shape
  K2, N = B.shape
  assert K == K2

  C = torch.zeros(M, N, device = 'cuda', dtype = torch.float32)

  A_ptr = ctypes.cast(A.data_ptr(), ctypes.POINTER(ctypes.c_float))
  B_ptr = ctypes.cast(B.data_ptr(), ctypes.POINTER(ctypes.c_float))
  C_ptr = ctypes.cast(C.data_ptr(), ctypes.POINTER(ctypes.c_float))

  lib_matmul.matmu_kernel(A_ptr, B_ptr, C_ptr, M, N, K)

  return C
