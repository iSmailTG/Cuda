
import sys, torch
sys.path.append('src')
from model import MultiHeadAttention

def test_shapes():

  hidden_size, num_heads = 128, 4
  batch, seq  = 2, 6

  attn = MultiHeadAttention(hidden_size, num_heads).cuda()
  x = torch.randn(batch, seq, hidden_size, device='cuda')
  output = attn(x)
  assert output.shape == (batch, seq, hidden_size), f"Shape expected: {batch, seq, hidden_size}, got: {output.shape}"
  print(f"Shape Test Passed: {output.shape}")

def casual_mask():
  hidden_size, num_heads= 128, 4
  seq = 6

  torch.manual_seed(42)
  attn = MultiHeadAttention(hidden_size, num_heads).cuda()

  x1 = torch.randn(1, seq, hidden_size, device='cuda')
  x2 = x1.clone()
  x2[0, -1, :] = torch.randn(hidden_size, device='cuda')

  out1 = attn(x1)
  out2 = attn(x2)

  diff = (out1 - out2).abs().max().item()

  assert diff < 1e-5, f"Token 0 should not change, diff: {diff:.6f}"
  print(f"Casual Mask Test passed, {diff:.2e}")

def no_nan():
  hidden_size, num_heads = 128, 4
  attn = MultiHeadAttention(hidden_size, num_heads).cuda()
  x = torch.randn(1, 32, hidden_size, device='cuda')
  out = attn(x)

  assert not torch.isnan(out), "NaN detected in attention output!"
  print("No NaN in output.")


if __name__=="__main__":
  test_shapes()
  casual_mask()
  no_nan()
  print("All tests Passed.")
