#include <cuda_runtime.h>

__global__ void matmul_kernel(float* A,float* B, float* C, M, N, K)
{
  int row = blockIdx.y * blockDim.y + threadIdx.y;
  int col = blockIdx.x * blockDim.y + threadIdx.x;

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
extern 'C'
{
  void matmul_cuda(float* A, float* B, float* C, M, N, K)
  {
    dim3 blockIdx(16, 16);
    dim3 blockDim((M + 15) / 16, (N + 15) / 16);

    matmul_kernel<<<blockIdx, blockDim>>>(A, B, C, M, N, K);
    cudaDeviceSynchronize();
  }
}
