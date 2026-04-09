# Day 2 Tracing shapes and understanding data transformations

## Why `.contiguous()` is needed before `.view()`?

```
batch = 2
seq = 4
num_heads = 3
hidden_size = 12
head_dim = hidden_size // num_heads

x = torch.randn(batch, seq, num_heads, head_dim)
x.is_contiguous()
--
True.
```


* The shape of output tensor -> `(batch, seq_len, num_heads, head_dim)`, and its layed out on physical memory by head like this:
  - `[head0, head1, head2]`
  - `[a0, a1, a2, a3, b0, b1, b2, b3, c0, c1, c2, c3]`  
         └─Head0──┘└─Head1──┘└─Head2──┘      ..
```
x = x.transpose(1, 2)
x.is_contiguous()
--
False
```
* After `.transpose()` the physical memory layout still the same, it just changes the stride pattern to reinterpret the same memory as if the data were rearranged, but doesn't actually move any data in memory.
* `view()` needs the memory layout to follow the default row-major stride pattern (contiguous).  
* That's why it refuses to work with it transposed non contiguous() data:

```
x = x.view(1, 5, 12)
RuntimeError: view size is not compatible with input tensor's size and stride (at least one dimension spans across two contiguous subspaces). Use .reshape(...) instead.
```
* That's why we need `.countiguous()`:

```
x = x.contiguous()
x = x.view(1, 5, 12)
x.shape
--
torch.Size([1, 5, 12])
```
* Then contiguous() will create a new tensor on memort that follows the row-major (C-type) rearranegement:

* Memory:  
[a0, b0, c0, a1, b1, c1, a2, b2, c2, a3, b3, c3]  
└─Row0─┘└─Row1┘└─Row2┘└─Row3─┘
---
##  Why we need Q.transpose(1, 2) after .view()?

* After `Q.view(batch, seq_len, num_heads, head_dim)` we apply `.transpose(1,2).
  - to get -> (batch, num_heads, seq_len, head_dim)
* Before doing attention matmul: `Q @ K` we need to swap `head` with `seq_len` in order to operate matmul on `(seq_len, head_dim)`independently for each head.  