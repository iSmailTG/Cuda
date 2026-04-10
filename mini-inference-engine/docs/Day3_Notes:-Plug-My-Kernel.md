## What Can Be Parallelized 
* In order to make use of my kernel into the multi head attention implementation, we need to understand that not all operations could benefit from kernel parallelism capabilities.
* In the implementation we have 2 matrix multiplications:  
  - `torch.matmul(Q, K.transpose(-2, -1))` & `torch.matmul(attn, V)`
* Other operations are either not compatible with the kernel at all, or will be covered later.
## How to deal with 4D matrix in 2D Matmul:
* If we re-check the shape of both of matmul's:
* Q @ K.T -> shape: 
  - **(batch, num_heads, seq, head_dim) @ (batch, num_heads, head_dim, seq)**
* So basically **4D** matrix. The problem is our `matmul.cu` can only handle 2D!
* The solution is to slice out each of those matrices to 2D matrix:
  - Q(seq, head_dim) @ K(head_dim, seq)
* Call the kernel, the reassemble them again to 4D matrices.
## Mental Model:

4D attention tensor  
       ↓  
  Python loop (batch × heads)  
       ↓  
  slice → 2D matrix           ← My kernel operates here  
       ↓  
  matmul_cuda(Q_2d, K_2d)  
       ↓  
  reassemble → 4D tensor  
  
## The Implementation:
```
scores_list = []
for b in range(batch_size):
  list_scores = []
  for h in range(self.num_heads):
    # shape -> (seq, head_dim), must be contiguous
    Q_2d = Q[b, h] 
    # shape -> (head_dim, seq), contiguous after transpose!
    K_2d = K[b, h].T.contiguous() 
    # shape -> (seq, seq)
    score = matmul_cuda(Q_2d, K_2d) / sqrt.math(self.head_dim) 
    # add a dimension at 0. shape -> (1, seq, seq)
    list_scores.append(score.unsqueeze(0)) 
  # concatenate all heads and add new dimension at 0. shape -> (1, head, seq, seq)
  scores_list.append(torch.cat(list_scores, dim=0).unsqueeze(0))
# concatenate all batches
# back to initial shape after applying matmuls -> (batch, heads, seq, seq)
scores = torch.cat(scores_list, dim=0)
```
* Same idea and staructure goes for `attn @ V`
