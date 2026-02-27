"""

https://en.wikipedia.org/wiki/Abel_polynomials

"""

from sympy import *
from sympy.abc import *
import sys
import os

def memoize(f, a):
    memo = {}
    def helper(x):
        if x not in memo:            
            memo[x] = expand(simplify(f(a, x)))
        return memo[x]
    return helper

def abel(a, n):
    if n < 0:
        return 0
    if n == 0:
        return 1
    if n == 1:
        return x
    return x*(x - a*n)**(n-1)

abel1 = memoize(abel, 1)
abel2 = memoize(abel, 2)

if len(sys.argv) < 2:
    print("usage: order ...")
    sys.exit()

for i in range(len(sys.argv)-1):
    n = int(sys.argv[i+1])
    print("Order {}".format(n))
    print("Abel 1")
    print(abel1(n))
    print("Abel 2")
    print(abel2(n))
    print()

