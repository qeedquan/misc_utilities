"""

https://en.wikipedia.org/wiki/Bessel_polynomials
https://en.wikipedia.org/wiki/Bessel_filter

"""

from sympy import *
from sympy.abc import *
import sys
import os

def memoize(f):
    memo = {}
    def helper(x):
        if x not in memo:            
            memo[x] = expand(simplify(f(x)))
        return memo[x]
    return helper

def pbessel(n):
    if n < 0:
        return 0
    if n == 0:
        return 1
    if n == 1:
        return x+1
    return (2*n-1)*x*pbessel(n-1) + pbessel(n-2)

def prbessel(n):
    if n < 2:
        return pbessel(n)
    return (2*n-1)*prbessel(n-1) + x*x*prbessel(n-2)

pbessel = memoize(pbessel)
prbessel = memoize(prbessel)

if len(sys.argv) < 2:
    print("usage: order ...")
    sys.exit()

for i in range(len(sys.argv)-1):
    n = int(sys.argv[i+1])
    print("Order {}".format(n))
    print("Bessel")
    print(pbessel(n))
    print("Reversed Bessel")
    print(prbessel(n))
    print()

