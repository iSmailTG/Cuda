
import torch
import torch.nn as nn
import math

BATCH = 1
SEQ = 6
HIDDEN_SIZE = 512
NUM_HEADS = 8
HEAD_DIM = HIDDEN_SIZE // NUM_HEADS

q_proj = nn.Linear(HIDDEN_SIZE, HIDDEN_SIZE, bias=False).cuda()
k_proj = nn.Linear(HIDDEN_SIZE, HIDDEN_SIZE, bias=False).cuda()
v_proj = nn.Linear(HIDDEN_SIZE, HIDDEN_SIZE, bias=False).cuda()

x = torch.randn(BATCH, SEQ, HIDDEN_SIZE, device='cuda')
print(f"x:       {tuple(x.shape)}")

Q = q_proj(x)
print(f"Q after q_proj: {tuple(Q.shape)}")
K = k_proj(x)
V = v_proj(x)

Q = Q.view(BATCH, SEQ, NUM_HEADS, HEAD_DIM)
print(f"Q after view: {tuple(Q.shape)}")
Q = Q.transpose(1, 2)
print(f"Q after transpose: {tuple(Q.shape)}")
K = K.view(BATCH, SEQ, NUM_HEADS, HEAD_DIM).transpose(1, 2)
V = V.view(BATCH, SEQ, NUM_HEADS, HEAD_DIM).transpose(1, 2)

print(f"K before transpose(-2, -1): {tuple(K.shape)}")
K_T = K.transpose(-2, -1)
print(f"K after transpose(-2, -1): {tuple(K_T.shape)}")

scores = torch.matmul(Q, K_T) / math.sqrt(HEAD_DIM)
print(f"scores shape: {tuple(scores.shape)}")


mask = torch.triu(torch.ones(SEQ, SEQ,device= x.device), diagonal= 1).bool()
scores = scores.masked_fill(mask, ("-inf"))

attn = torch.softmax(scores, dim=-1)
print(f"Attention Weights: {tuple(attn.shape)}")


out = torch.matmul(attn, V)
print(f"output: {tuple(out.shape)}")

out = out.transpose(1, 2).contiguous().view(BATCH, SEQ, HIDDEN_SIZE)
print(f"outputs after getting back to initial shape: {tuple(out.shape)}")
