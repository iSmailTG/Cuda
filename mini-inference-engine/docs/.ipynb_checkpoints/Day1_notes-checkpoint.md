# Day 1 — Attention Math Notes
## Write These In Your Own Words After Studying

---

## The Attention Formula

```
Attention(Q, K, V) = softmax( Q @ K^T / sqrt(d_k) ) @ V
```

### What Each Component Does

| Symbol | Name | Plain English meaning |
|---|---|---|
| `Q` | Queries | "What am I looking for?" — each token asks a question |
| `K` | Keys | "What do I offer?" — each token announces its content |
| `V` | Values | "What is my actual content?" — the info passed forward |
| `Q @ K^T` | Score matrix | Compare every query to every key → (seq_len × seq_len) |
| `/ sqrt(d_k)` | Scaling | Prevents large dot products pushing softmax into saturation |
| `softmax(...)` | Normalization | Turns raw scores into probabilities (each row sums to 1) |
| `@ V` | Weighted sum | Output = blend of values weighted by attention scores |

---

## Shapes Through the Forward Pass

Use concrete numbers: `batch=1, seq_len=8, hidden_size=512, num_heads=8, head_dim=64`

| Step | Variable | Shape | Notes |
|---|---|---|---|
| Input | `x` | `(1, 8, 512)` | batch, seq_len, hidden |
| After projection | `Q = q_proj(x)` | `(1, 8, 512)` | linear layer, same shape |
| After `.view(...)` | `Q` | `(1, 8, 8, 64)` | split hidden into heads × head_dim |
| After `.transpose(1,2)` | `Q` | `(1, 8, 8, 64)` | (batch, heads, seq, head_dim) |
| After transpose K | `K.T` | `(1, 8, 64, 8)` | for the matmul |
| Scores | `Q @ K^T` | `(1, 8, 8, 8)` | (batch, heads, seq, seq) |
| After softmax | `attn_weights` | `(1, 8, 8, 8)` | same as scores |
| Output | `weights @ V` | `(1, 8, 8, 64)` | (batch, heads, seq, head_dim) |
| Final | reshaped output | `(1, 8, 512)` | back to input shape |

---

## Multi-Head Attention — Why Multiple Heads?

_(Write your explanation here after studying)_

---

## Causal Masking — Why?

_(Write your explanation here after studying)_

### Why `-inf` and not `0`?

_(Write your explanation here)_

---

## Key Code Patterns to Remember

### The reshape + transpose pattern:
```python
# (batch, seq, hidden) → (batch, heads, seq, head_dim)
Q = Q.view(batch, seq_len, num_heads, head_dim).transpose(1, 2)
```

### Why `.contiguous()` before `.view()`:
- `.transpose()` returns a non-contiguous view (strides don't match row-major)
- `.view()` requires contiguous memory
- Pattern: `.transpose().contiguous().view(...)`

### The causal mask:
```python
mask = torch.triu(torch.ones(seq_len, seq_len), diagonal=1).bool()
scores = scores.masked_fill(mask, float("-inf"))
```
- `triu(diagonal=1)` → True for upper triangle (future positions)
- `masked_fill(-inf)` → those become ~0 after softmax

---

## Questions to Be Able to Answer Out Loud

1. What is Q, K, V in plain English?
2. Why do we scale by `sqrt(head_dim)`?
3. What is causal masking and why do we use `-inf` instead of `0`?
4. Why do we split into multiple heads?
5. What shape is the score matrix and why does that make attention O(n²)?
6. Why do we call `.contiguous()` before `.view()`?

---

## My Own Notes

_(Space for anything you want to add as you study)_
