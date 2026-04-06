import sys, torch
sys.path.append("temp")
from kernel_ext import matmul_cuda_V2

def bench(fn, A, B, label, n=50):
  M, K = A.shape
  K2, N = A.shape

  flops = 2 * M * K * N

  for _ in range(100): fn(A, B)
  torch.cuda.synchronize()

  start = torch.cuda.Event(enable_timing= True)
  end = torch.cuda.Event(enable_timing= True)
  start.record()
  for _ in range(n): fn(A, B)
  end.record()
  torch.cuda.synchronize()
  ms = start.elapsed_time(end)
  gflops = flops / (ms / 1000) / 1e9

  print(f"{label:35s}: {ms:7.3f} ms | {gflops:8.1f} GFLOPS/s")
  return ms, gflops

print(f"{'operation':<35} {'time':>7} {'GFLOPS/s':>10}")
print("-" * 60)

M, N, K= 512, 512, 512                          
A = torch.randn(M, K, device='cuda', dtype = torch.float32)
B = torch.randn(K, N, device='cuda', dtype = torch.float32)

t1, g1 = bench(matmul_cuda_V2, A, B, f"My Kernel: ({M} *{ K}) @ ({K}*{N})")
t2, g2 = bench(torch.matmul, A, B, f"Pytorch CuBLAS: ({M} * {K}) @ ({K} * {N}) ")
print(f"\nSpeedup factor: {g2/g2:.1f}x")
print(f"My kernel uses {g1/g2*100:.1f}% of compute that Pytorch uses")
