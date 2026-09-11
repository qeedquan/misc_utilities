from math import *

"""
https://en.wikipedia.org/wiki/Stirling%27s_approximation

A good approximation of the factorial function in the sense
that stirling(n)/factorial(n) = 1 as n -> infinity
but their differences grows without bound
"""

def stirling(n):
    return n**n * exp(-n) * sqrt(2*pi*n)

for i in range(1, 101):
    s = stirling(i)
    f = factorial(i)
    print(i, s, f, s/f, abs(s-f))

