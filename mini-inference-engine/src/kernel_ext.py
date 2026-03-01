
from torch.utils.cpp_extension import load

try:
  matmul_ext = load(name = "matmul_ext", sources = ['kernels/matmul_ext.cu'], verbose = True)
  print("Compilation Completed")
except Exception as e:
    print(f"An error occurred during compilation: {e}")

def matmul_cuda_v2(A, B):
  return matmul_ext.matmul(A, B)
