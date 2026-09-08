
import torch, sys
sys.path.append('src')
from kernel_wrapper import matmul_cuda

def test_correctness():
  A = torch.randn(64, 128, device = 'cuda', dtype= torch.float32)
  B = torch.randn(128, 64, device = 'cuda', dtype= torch.float32)

  C = matmul_cuda(A, B)
  C_ref = torch.matmul(A, B)
  diff = (C - C_ref).abs().max().item()
  print(f"Max absolute diff vs Pytorch: {diff:.6f}")
  assert diff < 1e-3
  print("Pass Correctness")

def test_shapes():
  for M, K, N in ([32, 64, 16], [64, 128, 32], [512, 512, 512]):
    A = torch.randn(M, K, device = 'cuda', dtype = torch.float32)
    B = torch.randn(K, N, device = 'cuda', dtype = torch.float32)

    C = matmul_cuda(A, B)
    C_ref = torch.matmul(A, B)
    assert C.shape == (M, N)
    diff = (C - C_ref).abs().max().item()
    assert diff < 1e-3
    print(f"PASS - ({M}, {K}) @ ({K}, {N}) = ({M}, {N}), max_diff = {diff:.6f}")

if __name__ == "__main__":
  test_correctness()
  print()
  test_shapes()
  print("\nAll Tests Passed.")

