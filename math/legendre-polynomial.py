"""

https://en.wikipedia.org/wiki/Legendre_polynomials

"""

from sympy import *
from sympy.abc import *
import sys
import os

def legendre(n):
    if n < 0:
        return 0
    p = 0
    for k in range(n+1):
        p += binomial(n, k) * binomial(n+k, k) * ((x-1)/2)**k
    return expand(simplify(p))

def shifted_legendre(n):
    if n < 0:
        return 0
    p = 0
    for k in range(n+1):
        p += binomial(n, k) * binomial(n+k, k) * (-x)**k
    return expand(simplify(-1**n * p))

def associated_legendre(m, l):
    p = 0
    for k in range(m, l+1):
        p += factorial(k)/factorial(k-m) * x**(k-m) * binomial(l, k) * binomial((l+k-1)/2, l) 
    return expand(simplify(-1**m * 2**l * (1-x*x)**(m/2) * p))

if len(sys.argv) < 2:
    print("usage: order ...")
    sys.exit()

for i in range(len(sys.argv)-1):
    n = int(sys.argv[i+1])
    print("Order {}".format(n))
    print("Legendre")
    print(legendre(n))
    print("Shifted Legendre")
    print(shifted_legendre(n))
    for i in range(n+1):
        print("Associated Legendre {} {}".format(i, n))
        print(associated_legendre(i, n))
    print()

