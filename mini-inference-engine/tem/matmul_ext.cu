#include <torch/extension.h>

__global__ void matmul_kernel(float* A,float* B, float* C, int M, int N, int K)
{
  int row = blockIdx.y * blockDim.y + threadIdx.y;
  int col = blockIdx.x * blockDim.x + threadIdx.x;

  if(row < M && col < N)
  {
    float sum = 0.0f;
    for(int i = 0; i < K; i++)
    {
      sum += A[row * K + i] * B[i * N + col];
    }
    C[row * N + col] = sum;

  }
}

torch::Tensor matmul_ext(torch::Tensor A, torch::Tensor B)
{
  TORCH_CHECK(A.is_cuda(), "must be on cuda");
  TORCH_CHECK(B.is_cuda(), "must be on cuda");
  TORCH_CHECK(A.dtype() == torch::kFloat32, "support float32 only");
  TORCH_CHECK(A.is_contiguous(), "must be contiguous");
  TORCH_CHECK(B.is_contiguous(), "must be contiguous");
  TORCH_CHECK(A.size(1) == B.size(0), "A col mus match ");

  int M = A.size(0), K = A.size(1), N = B.size(1);
  auto C = torch::zeros({M, N}, A.options());

  dim3 blockDim(16, 16);
  dim3 gridDim((N + 15) / 16, (M + 15) / 16);

  matmul_kernel<<<gridDim, blockDim>>>(
    A.data_ptr<float>(),
    B.data_ptr<float>(),
    C.data_ptr<float>(),
    M, N, K);

  cudaDeviceSynchronize();
  return C;
}

PYBIND11_MODULE(TORCH_EXTENSION_NAME, m)
{
  m.def("matmul", &matmul_ext, "Matrix Multiply (CUDA)");
}
