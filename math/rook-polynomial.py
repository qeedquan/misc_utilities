"""

https://en.wikipedia.org/wiki/Rook_polynomial

"""

import sys
from sympy import *
from sympy.abc import x

def rook(m, n):
    s = 0
    for k in range(min(m, n)+1):
        s += binomial(m, k)*binomial(n, k)*factorial(k)*x**k
    return s

if len(sys.argv) < 3:
    print("usage: m n")
    sys.exit()

m = int(sys.argv[1])
n = int(sys.argv[2])
print(rook(m, n))

