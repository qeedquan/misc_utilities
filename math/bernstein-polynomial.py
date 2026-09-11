"""

https://en.wikipedia.org/wiki/Bernstein_polynomial

"""

from sympy import *
from sympy.abc import *
import sys
import os

def bernstein(v, n):
    return expand(binomial(n, v) * x**v * (1-x)**(n-v))

def bernstein_derivative(v, n):
    return n*(bernstein(v-1, n-1) - bernstein(v, n-1))

def bernstein_integral(v, n):
    s = 0
    for j in range(v+1, n+2):
        s += bernstein(j, n+1)
    return s/(n+1)

if len(sys.argv) < 3:
    print("usage: v n")
    sys.exit()

v = int(sys.argv[1])
n = int(sys.argv[2])

print("Bernstein(v={}, n={})".format(v, n))
print(bernstein(v, n))
print()
print("Derivative(v={}, n={})".format(v, n))
print(bernstein_derivative(v, n))
print()
print("Integral(v={}, n={})".format(v, n))
print(bernstein_integral(v, n))
