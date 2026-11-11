import torch
import torch.nn as nn
from math import floor

# To preserve the same height/width, use padding = kernel_size // 2
# P = padding
# K = kernel size
# S = stride
def conv2d_dim(C_in, H_in, W_in, P=0, K=1, S=1):
    C_out = C_in
    H_out = floor((H_in + 2*P - K) / S) + 1
    W_out = floor((W_in + 2*P - K) / S) + 1
    return (C_out, H_out, W_out)

def maxpool2d_dim(C_in, H_in, W_in, K=1, S=1):
    C_out = C_in
    H_out = floor((H_in - K) / S) + 1
    W_out = floor((W_in - K) / S) + 1
    return (C_out, H_out, W_out)

def flatten_dim(C_in, H_in, W_in):
    return C_in * H_in * W_in

# 1x600x600
t = torch.randn(1, 600, 600)
print(t.shape)
print()

# input is 1 channel
# output is 32 channels
# kernel size 3x3
# output is: 32x598x598 
C = nn.Conv2d(1, 32, 3)
print(conv2d_dim(32, 600, 600, K=3))
t = C(t)
print(t.shape)
print()

# output is: 32x299x299
P = nn.MaxPool2d(2, 2)
print(maxpool2d_dim(32, 598, 598, K=2, S=2))
t = P(t)
print(t.shape)
print()

# input is 32 channels
# output is 16 channels
# kernel size is 3x3
# output is: 32x297x297
C = nn.Conv2d(32, 16, 3)
print(conv2d_dim(16, 299, 299, K=3))
t = C(t)
print(t.shape)
print()

# input is 16 channel
# output is 8 channel
# kernel size is 3x3
# output is 8x295x295
C = nn.Conv2d(16, 8, 3)
print(conv2d_dim(8, 297, 297, K=3))
t = C(t)
print(t.shape)
print()

# 8x87025
F = nn.Flatten()
t = F(t)
print(t.shape)

# 8x128
L = nn.Linear(295 * 295, 128)
t = L(t)
print(t.shape)

