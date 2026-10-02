"""

https://en.wikipedia.org/wiki/Hermite_polynomials

"""

from sympy import *
from sympy.abc import *
import sys
import os

def hermite_physicist(n):
    if n < 0:
        return 0
    if n == 0:
        return 1
    p = 0
    for m in range (n//2+1):
        p += (-1)**m / (factorial(m)*factorial(n - 2*m)) * (2*x)**(n - 2*m)
    p *= factorial(n)
    return expand(simplify(p))

if len(sys.argv) < 2:
    print("usage: order ...")
    sys.exit()

for i in range(len(sys.argv)-1):
    n = int(sys.argv[i+1])
    print("Order {}".format(n))
    print("Hermite (Physicist Version)")
    print(hermite_physicist(n))
    print()

