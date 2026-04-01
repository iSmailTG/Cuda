#include <cuda_runtime.h>

__global__ void kernel_matmul(float* A, float* B, float* C, M, N, K)
{
  int row = blockIdx.y * blockDim.y + threadIdx.y;
  int col = blockIdx.x * bloxkDim.x + threadIdx.x;

  if(row < M && col < N)
  {
    float sum = 0.0f;
    for(int i = 0; i < K; i++)
    {
      sum += A[row * K + i] + B[i * N + col];
    }
    C[row * N + col] = sum;
  }
}

extern "C"
{
  void matmul_cuda(float* A, float* B, float* C, M, N, K)
  {
    dim3 blockIdx(16, 16);
    dim3 blockDim((M + 15) / 16, (N + 15) / 16);

    kernel_matmul<<<blockIdx, blockDim>>>(A, B, M, N, K);
    cudaDeviceSynchronize();
  }
}
