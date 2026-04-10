
import torch
import math
import torch.nn as nn
import os
import ctypes
import subprocess

if not os.path.exists('./kernels/matmul.so'):
  subprocess.run(['nvcc','-shared', '-o', './kernels/matmul.so', './kernels/matmul.cu', '-Xcompiler', '-fPIC', '-O2'])

matmul_lib = ctypes.CDLL('./kernels/matmul.so')
matmul_lib.matmul_cuda.argtypes = [
    ctypes.POINTER(ctypes.c_float),
    ctypes.POINTER(ctypes.c_float),
    ctypes.POINTER(ctypes.c_float),
    ctypes.c_int, ctypes.c_int, ctypes.c_int]
matmul_lib.matmul_cuda.restype = None

def matmul_cuda(A: torch.Tensor, B: torch.Tensor) -> torch.Tensor:
  assert A.is_cuda, "must be on Cuda"
  assert B.is_cuda, "must be on Cuda"
  assert A.dtype == torch.float32, "support float32 only"
  assert A.is_contiguous() and B.is_contiguous(), "must be contiguous"
  
  M, K = A.shape
  K2, N = B.shape
  assert K == K2, f"inner dims must match {K} X {K2}"

  C = torch.zeros(M, N, device="cuda", dtype = torch.float32)

  A_ptr = ctypes.cast(A.data_ptr(), ctypes.POINTER(ctypes.c_float))
  B_ptr = ctypes.cast(B.data_ptr(), ctypes.POINTER(ctypes.c_float))
  C_ptr = ctypes.cast(C.data_ptr(), ctypes.POINTER(ctypes.c_float))
  matmul_lib.matmul_cuda(A_ptr, B_ptr, C_ptr, M, N, K)
  return C


class MultiHeadAttention(nn.Module):
  def __init__(self, hidden_size, num_heads):
    super().__init__()
    self.hidden_size = hidden_size
    self.num_heads = num_heads
    self.head_dim = hidden_size // num_heads

    self.q_proj = nn.Linear(hidden_size, hidden_size, bias=False).cuda()
    self.k_proj = nn.Linear(hidden_size, hidden_size, bias=False).cuda()
    self.v_proj = nn.Linear(hidden_size, hidden_size, bias=False).cuda()
    self.o_proj = nn.Linear(hidden_size, hidden_size, bias=False).cuda()


  def forward(self, x):

    batch, seq_len, _ = x.shape

    Q = self.q_proj(x).view(batch, seq_len, self.num_heads, self.head_dim).transpose(1, 2)
    K = self.k_proj(x).view(batch, seq_len, self.num_heads, self.head_dim).transpose(1, 2)
    V = self.v_proj(x).view(batch, seq_len, self.num_heads, self.head_dim).transpose(1, 2)

    scores_list = []
    for b in range(batch):
      head_scores = []
      for h in range(self.num_heads):
        Q_2d = Q[b, h].contiguous() # shape -> (seq_len, head_dim)
        K_2d = K[b, h].contiguous().T # shape -> (head_dim, seq_len)
        score = matmul_cuda(Q_2d, K_2d) / math.sqrt(self.head_dim)
        head_scores.append(score.unsqueeze()) # shape -> (1, seq_len, seq_len)
      scores_list.append(torch.cat(head_scores, dim=0).unsqueeze(0)) # shape -> (1, head, seq_len, seq_len)
    scores = torch.cat(scores_list, dim=0) # shape -> (batch, head, seq_len, seq_len)

    mask = torch.triu(torch.ones(seq_len, seq_len, device = x.device), diagonal= 1).bool()
    scores = scores.masked_fill(mask, float('-inf'))

    attn = torch.softmax(scores, dim= -1)

    # attn @ V
    outs_list = []
    for b in range(batch):
      head_outs = []
      for h in range(self.num_heads):
        attn_2d = attn[b, h].contiguous() # shape -> (seq_len, seq_len)
        V_2d = V[b, h].contiguous() # shape -> (seq_len, head_dim) -> both same inner dimensionm no need to transpose V
        out_score = matmul_cuda(attn_2d, V_2d) # shape -> (seq_len, head_dim)
        head_outs.append(out_score.unsqueeze(0)) # shape -> (1, seq_len, head_dim)
      outs_list.append(torch.cat(head_outs, dim=0).unsqueeze(0)) # shape -> (1, head, seq_len, head_dim)
    outputs = torch.cat(outs_list, dim=0) # shape -> (batch, head, seq_len, head_dim)

    outputs = outputs.transpose(1, 2).contiguous().view(batch, seq_len, self.hidden_size)
    outputs = self.o_proj(outputs)

    return outputs

