#!/usr/bin/env python3

"""

Common forms of the laplace transform and its inverse

"""

from sympy.integrals import inverse_laplace_transform
from sympy.abc import *
from sympy import *

"""

1/(s-1) + 1/(s-2) .. 1/(s-n) -> [e^t + e^(2*t) + e^(3*t) + ... e^(n*t)]
1/(s+1) + 1/(s+2) .. 1/(s+n) -> [e^(-t) + e^(-2*t) + e^(-3*t) + ... e^(-n*t)]

"""

G1 = 0
G2 = 0
for i in range(1, 10):
    G1 += 1/(s - i)
    G2 += 1/(s + i)

print(G1)
print(expand(inverse_laplace_transform(G1, s, t)))
print()
print(G2)
print(expand(inverse_laplace_transform(G2, s, t)))
print()

"""

1 / ((s-1)*(s-2)*(s-3)*...(s-n))

"""
G = 1
for i in range(1, 10):
    G *= 1/(s - i)
print(G)
print(expand(inverse_laplace_transform(G, s, t)))
print()

"""

(s-1)*(s-2)*...(s-n) = 0*t
(s+1)*(s+2)*...(s+n) = n! * delta_function(t)

"""
G1 = 1
G2 = 1
for i in range(1, 10):
    G1 *= (s - i)
    G2 *= (s + i)
print(G1)
print(expand(inverse_laplace_transform(G1, s, t)))
print(G2)
print(expand(inverse_laplace_transform(G2, s, t)))
print()

"""

First order
K/(T*s + 1) -> K*exp(-t/T)/T

"""
G = K/(T*s + 1)
print(expand(inverse_laplace_transform(G, s, t)))
print()

"""

Second order
K*W^2 / (s^2 + 2*Z*W*s + W^2) -> K*W^2 * exp(-W*Z*t) * sin(t*sqrt(-W^2*Z^2 + W^2)) / sqrt(-W^2*Z^2 + W^2)

"""
G =  K*W*W / (s*s + 2*Z*W*s + W*W)
print(expand(inverse_laplace_transform(G, s, t)))
print()

