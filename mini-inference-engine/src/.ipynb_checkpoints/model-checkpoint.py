
import sys
import torch
import torch.nn as nn
import math


class MultiHeadAttention(nn.Module):
  def __init__(self, hidden_size, num_heads):
    super().__init__()
    self.hidden_size = hidden_size
    self.num_heads = num_heads
    self.head_dim = hidden_size // num_heads

    self.q_proj= nn.Linear(hidden_size, hidden_size, bias=False)
    self.k_proj= nn.Linear(hidden_size, hidden_size, bias=False)
    self.v_proj= nn.Linear(hidden_size, hidden_size, bias=False)
    self.o_proj= nn.Linear(hidden_size, hidden_size, bias=False)


  def forward(self, x):
    # x: batch_size, seq_len, hidden_size

    batch_size, seq_len, _ = x.shape


    # (batch, seq_len, hidden_size)
    Q = self.q_proj(x)
    K = self.k_proj(x)
    V = self.v_proj(x)

    # reshape it for multi-head attention:
    # from (batch, seq_len, hidden_size)
    # to (batch, seq_len, num_heads, head_dim) with view, then:
    # transpose it to: (batch, num_heads, seq_len, head_dim)

    Q = Q.view(batch_size, seq_len, self.num_heads, self.head_dim).transpose(1, 2)
    K = K.view(batch_size, seq_len, self.num_heads, self.head_dim).transpose(1, 2)
    V = V.view(batch_size, seq_len, self.num_heads, self.head_dim).transpose(1, 2)

    # Attention score:
    # Q @ K^T 
    # Q&K.shape => [batch, num_heads, seq_len, head_dim]
    # K^T.shape => [batch, num_heads, head_dim, seq_len]
    # Q @ K^T => [batch, num_heads, seq_len, seq_len]

    scores = torch.matmul(Q, K.transpose(-2, -1)) / math.sqrt(self.head_dim)

    # Causual Mask:
    # prvent input token i to attent token j if j > i
    mask = torch.triu(torch.ones(seq_len, seq_len, device = x.device), diagonal=1).bool()
    scores = scores.masked_fill(mask, float('-inf'))

    # Softmax
    attn_weights = torch.softmax(scores, dim=-1)

    # Weighted Sum of Values:
    # attn_wights.shape -> [batch, num_heads, seq_len, seq_len]
    # V.shape -> [batch, num_heads, seq_len, head_dim]
    # output.shape -> [batch, num_heads, seq_len, head_dim]

    output = torch.matmul(attn_weights, V)

    # reshape back
    # from -> [batch, num_heads, seq_len, head_dim ]
    # to -> [batch, seq_len, num_heads, head_dim] after transpose(1, 2).contiguous()
    # to final shape: -> [batch, seq_len, hidden_size]

    output = output.transpose(1, 2).contiguous()
    output = output.view(batch_size, seq_len, self.hidden_size)

    output = self.o_proj(output)

    return output
