#include <cuda_runtime.h>


__global__ void kernel_matmul(float* A, float* B, float* C,
                              int M, int N, int K)
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

extern "C"
{
  void matmul_cuda(float* A, float* B, float* C,
                   int M, int N, int K)
  {
    dim3 blockIdx(16, 16);
    dim3 blockDim((N + 15) / 16, (M + 15) / 16);

    kernel_matmul<<< blockDim, blockIdx >>>(A, B, C, M, N, K);
    cudaDeviceSynchronize();

  }
}
