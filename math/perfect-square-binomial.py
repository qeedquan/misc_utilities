"""

A perfect square binomial is a quadratic function that can be factored into the following

a*x^2 + b*x + c = (u*x + v)^2

If we are less strict we can allow

a*x^2 + b*x + c = w*(u*x + v)^2

The first condition can be satisfied when the discriminant is 0 and the gcd of a, b, c is 1
d = b*b - 4*a*c = 0
gcd(a, gcd(a, b)) = 1

If we want the second form, we can get rid of the requirement of the gcd

"""

from sympy.abc import x
import sympy
import numpy as np
import sys

def str2bool(s):
    return s.lower() in ['true', '1', 't', 'y']

def is_perfect_square_binomial(a, b, c, strict):
    d = b*b - 4*a*c
    if d != 0:
        return False
   
    if strict and np.gcd(a, np.gcd(b, c)) != 1:
        return False

    return True

def find(n, strict):
    for a in range(1, n+1):
        for b in range(1, n+1):
            for c in range(1, n+1):
                if is_perfect_square_binomial(a, b, c, strict):
                    s = a*x*x + b*x + c
                    print("{} | {} | ({},{},{})".format(sympy.factor(s), s, a, b, c))

if len(sys.argv) < 3:
    print("usage: <n> <strict>")
    sys.exit(2)

find(int(sys.argv[1]), str2bool(sys.argv[2]))
