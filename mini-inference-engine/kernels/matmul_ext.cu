#include <torch/extension.h>
__global__ void matmul_kernel(float* A, float* B, float* C, int M, int N, int K)
{
  int row = blockIdx.y * blockDim.y + threadIdx.y;
  int col = blockIdx.x * blockDim.x + threadIdx.x;

  if(row < M && col < N)
  {
    float sum = 0.0f;
    for(int i = 0; i < K; i++)
    {
      sum += [row * K + i] * [i * N + col];
    }
    C[row * N + col] = sum;
  }
}

torch::Tensor matmul_cuda_ext(torch::Tensor A, torch::Tensor B)
{
  TORCH_CHECK(A.is_cuda(), "A must be on GPU");
  TORCH_CHECK(B.is_cuda(), "B must be on GPU");
  TORCH_CHECK(A.dtype == torch::kFloat32, "Supports float32 only");
  TORCH_CHECK(A.is_contiguous(), "A must be contiguous");
  TORCH_CHECK(B.is_contiguous(), "B must be contiguous");
  TORCH_CHECK(A.size(1) == B.size(0), "Matrices inner values must be equal");

  int M = A.size(0), k = A.size(1), N = B.size(1);
  aut C = torch::zeros({M, N}, A.options());

  dim3 blockDim(16, 16);
  dim3 gridDim((N + 15) / 16, (M +15) / 16);
  matmul_kernel <<<gridDim, blockDim>>>(A.data_ptr<float>(),
                                        B.data_ptr<float>(),
                                        C.data_ptr<float>(),
                                        M, N, K);
  cudaDeviceSynchronize();
  return C;
}
PYBIND11_MODULE(TORCH_EXTENSION_NAME, m)
{
  m.def("matmul", &matmul_cuda_ext, "Matrix multiply (Cuda)");
}
