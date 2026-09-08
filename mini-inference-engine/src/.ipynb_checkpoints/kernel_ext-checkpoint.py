from torch.utils.cpp_extension import load


matmul_ext = load(
    name    = "matmul_ext",
    sources = ["kernels/matmul_ext.cu"],
    verbose = True,
)

def matmul_cuda_v2(A, B):
   
    return matmul_ext.matmul(A, B)
