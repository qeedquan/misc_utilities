"""

https://en.wikipedia.org/wiki/Laguerre_polynomials

"""

from sympy import *
from sympy.abc import *
import sys
import os

def memoize(f, a):
    memo = {}
    def helper(a, x):
        if (a, x) not in memo: 
            memo[(a, x)] = expand(simplify(f(a, x)))
        return memo[(a, x)]
    return helper

def plaguerre_general(a, n):
    if n < 0:
        return 0
    if n == 0:
        return 1
    if n == 1:
        return 1+a-x
    return ((2*(n-1)+1+a-x)*plaguerre_general(a, n-1) - (n-1+a)*plaguerre_general(a, n-2)) / (n)

def plaguerre(n):
    return plaguerre_general(0, n)

plaguerre_general = memoize(plaguerre_general, a)

if len(sys.argv) < 2:
    print("usage: order ...")
    sys.exit()

for i in range(len(sys.argv)-1):
    n = int(sys.argv[i+1])
    print("Order {}".format(n))
    print("Laguerre")
    print(plaguerre(n))
    print("Generalized Laguerre")
    print(plaguerre_general(Symbol('a'), n))
    print()

