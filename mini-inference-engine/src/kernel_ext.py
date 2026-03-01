
from torch.utils.cpp_extension import load
matmul_ext = load(name = "matmul_ext", source = ['kernel/matmul_ext.cu'], verbose = True)

def matmul_cuda_v2(A, B):
  return matmul_ext.matmul(A, B)
