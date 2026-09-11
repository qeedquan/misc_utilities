"""

https://en.wikipedia.org/wiki/Spherical_harmonics
https://en.wikipedia.org/wiki/Table_of_spherical_harmonics

"""

from sympy import *
from sympy.abc import *
import sys
import os

def associated_legendre(m, l):
    p = 0
    for k in range(m, l+1):
        p += factorial(k)/factorial(k-m) * x**(k-m) * binomial(l, k) * binomial((l+k-1)/2, l) 
    return expand(simplify(-1**m * 2**l * (1-x*x)**(m/2) * p))

def associated_legendre_full(m, l):
    k = 1
    if m < 0:
        m = -m
        k = (-1)**m * factorial(l-m)/factorial(l+m)
    return k*associated_legendre(m, l)

# the x in the legendre polynomial is defined to be cos(theta)
def spherical(m, l, k):
    theta = Symbol('theta')
    phi = Symbol('phi')
    P = associated_legendre_full(m, l)
    s = (k*P)*exp(I*m*phi)
    s = s.subs(x, cos(theta))
    s = simplify(s)
    return s

def acoustic(m, l):
    k = sqrt((2*l+1)/(4*pi) * factorial(l-m)/factorial(l+m))
    return spherical(m, l, k)

def geodesy(m, l):
    k = sqrt((2*l+1) * factorial(l-m)/factorial(l+m))
    return spherical(m, l, k)

def magnetics(m, l):
    k = sqrt(factorial(l-m)/factorial(l+m))
    return spherical(m, l, k)

# the condon-shortley phase refers to the factor (-1)^m 
def quantum(m, l):
    return (-1)**m * acoustic(m, l)

if len(sys.argv) < 3:
    print("usage: m l")
    sys.exit()

m = int(sys.argv[1])
l = int(sys.argv[2])
init_printing()
print("m={} l={}".format(m, l))
print("Acoustic Spherical Harmonics")
pprint(acoustic(m, l))
print("Quantum Spherical Harmonics (Condon-Shortley Phase)")
pprint(quantum(m, l))
print("Geodesy Spherical Harmonics")
pprint(geodesy(m, l))
print("Magnetic Spherical Harmonics")
pprint(magnetics(m, l))

