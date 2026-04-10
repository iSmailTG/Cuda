
* In order to make use of my kernel into the multi head attention implementation, we need to understand that not all operations could benefit from kernel parallelism capabilities.
* In the implementation we have 2 matrix multiplications:  
  - `torch.matmul(Q, K.transpose(-2, -1))` & `torch.matmul(attn, V)`
* Other operations are either not compatible with the kernel at all, or will be covered later.
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
  slice → 2D matrix           ← your kernel operates here  
       ↓  
  matmul_cuda(Q_2d, K_2d)  
       ↓  
  reassemble → 4D tensor  
