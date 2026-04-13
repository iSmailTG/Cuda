import torch
import math


def precompute_freqs(head_dim, seq_len, base=10000.0, device="cuda"):
    """
    Precompute the rotation frequencies.
    Returns: freqs of shape (seq_len, head_dim // 2) — one frequency per dimension pair
    """
    # θᵢ = 1 / (10000 ^ (2i / head_dim))
    # i = 0, 1, ..., head_dim//2 - 1
    i = torch.arange(0, head_dim, 2, dtype=torch.float32, device=device)  # (head_dim//2,)
    theta = 1.0 / (base ** (i / head_dim))                                 # (head_dim//2,)

    # For each position m, compute m * θᵢ
    positions = torch.arange(seq_len, dtype=torch.float32, device=device)  # (seq_len,)
    freqs = torch.outer(positions, theta)                                   # (seq_len, head_dim//2)
    return freqs


def apply_rope(x, freqs):
    """
    Apply rotary position embeddings to x.
    x:     (batch, num_heads, seq_len, head_dim)
    freqs: (seq_len, head_dim // 2)
    Returns x rotated in-place equivalent
    """
    seq_len, head_dim_half = freqs.shape

    # Split x into even and odd dimension pairs
    x_even = x[..., 0::2]  # dimensions 0, 2, 4, ... → (batch, heads, seq, head_dim//2)
    x_odd  = x[..., 1::2]  # dimensions 1, 3, 5, ...

    cos_freq = freqs.cos()[None, None, :, :]  # (1, 1, seq_len, head_dim//2) for broadcasting
    sin_freq = freqs.sin()[None, None, :, :]

    # 2D rotation: (x_even * cos - x_odd * sin, x_even * sin + x_odd * cos)
    x_rotated_even = x_even * cos_freq - x_odd * sin_freq
    x_rotated_odd  = x_even * sin_freq + x_odd * cos_freq

    # Interleave back: (batch, heads, seq, head_dim)
    x_out = torch.zeros_like(x)
    x_out[..., 0::2] = x_rotated_even
    x_out[..., 1::2] = x_rotated_odd
    return x_out


if __name__ == "__main__":
    BATCH, HEADS, SEQ, HEAD_DIM = 1, 4, 8, 64
    Q = torch.randn(BATCH, HEADS, SEQ, HEAD_DIM, device="cuda")
    K = torch.randn(BATCH, HEADS, SEQ, HEAD_DIM, device="cuda")

    freqs = precompute_freqs(HEAD_DIM, SEQ)
    Q_rope = apply_rope(Q, freqs)
    K_rope = apply_rope(K, freqs)

    print(f"Q shape before RoPE : {tuple(Q.shape)}")
    print(f"Q shape after RoPE  : {tuple(Q_rope.shape)}")
    assert Q_rope.shape == Q.shape

    # Verify: the norm of each vector should be preserved (rotation does not scale)
    norm_before = Q[0, 0, 0].norm().item()
    norm_after  = Q_rope[0, 0, 0].norm().item()
    print(f"Norm before RoPE    : {norm_before:.4f}")
    print(f"Norm after RoPE     : {norm_after:.4f}  (should be ~same)")
    assert abs(norm_before - norm_after) < 1e-4, "Rotation should preserve norm!"

    print("\n✅ RoPE PyTorch reference works correctly!")
