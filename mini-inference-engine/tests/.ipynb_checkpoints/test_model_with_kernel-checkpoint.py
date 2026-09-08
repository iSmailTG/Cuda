
import torch
import sys

sys.path.append('src')
from model_with_kernel import MultiHeadAttention as mha

def test_shape():
  hidden, heads = 512, 8
  batch, seq = 12, 6

  attn = mha(hidden, heads).cuda()
  x = torch.randn(batch, seq, hidden, device='cuda')

  out = attn(x)

  assert out.shape == (batch, seq, hidden), f'output shape must match: {batch, seq, hidden}, you got: {out.shape}'
  print("shape test passed")


def masking():
  hidden, heads= 512, 8
  seq = 6

  attn = mha(hidden, heads).cuda()

  x1 = torch.randn(1, seq, hidden, device='cuda')
  x2 = x1.clone()
  x2[0, -1, :] = torch.randn(hidden, device='cuda')

  out1 = attn(x1)
  out2 = attn(x2)

  diff = (out1[0,0] - out2[0,0]).abs().max().item()

  assert diff < 1e-5, f"Token 0 should not changed: {diff:.6}"
  print("Checking masking test: Passed")

def is_nan():
  hidden, heads= 512, 8
  attn = mha(hidden, heads).cuda()

  x = torch.randn(1, 32, hidden, device="cuda")
  out = attn(x)
  assert not torch.isnan(out).any(), "NaN value detected"
  print("NaN test passed")

if __name__=="__main__":
  test_shape()
  masking()
  is_nan()
  print("All Tests Passed")
