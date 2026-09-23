from sympy.abc import r 
from sympy import *

"""
N-dimensional volume of a sphere derivation:

ND cube volume = r^n
ND sphere volume = C*r^n
where C is a proportionality constant, we need to derive
the equation for C.

Let V_n(r) be a function that returns the volume of a sphere in ND

The surface is then
dV_n(r)/dr = n * C_n * r^(n-1)

hence the volume of the elements are
dV_n(r)/dr * dr = n * C_n * r^(n-1) * dr

Set r^2 to be t and we can see the following relation to the gamma function

Cn = pi^(n/2) / Gamma(0.5*n + 1)

Use recurrence relation and get
C(n) = 2pi/n * C(n-2)

as n -> infinity, C(n) -> 0, this means that the volume of a sphere approaches 0
as the dimension increases, this tells you that the all the volume exists on its surface
"""

pi = Symbol('pi')

def C(n):
    if n <= 0:
        return 0
    
    # we can think of a 1d sphere as a line, so its area is the positive and the negative part
    # of the line, hence why it is 2
    if n == 1:
        return 2

    if n == 2:
        return pi
    
    return 2*pi/n * C(n-2)

def V(n):
    return simplify(r**n * C(n))

for i in range(1, 101):
    print(i, V(i))

