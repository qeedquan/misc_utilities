"""

https://en.wikipedia.org/wiki/Companion_matrix
http://et.engr.iupui.edu/~skoskie/ECE602/LNotes/A_Companion_Matrix.pdf

"""

import numpy as np
from sympy import *
from sympy.abc import *
from sympy.solvers import solve

"""

A companion matrix is of the form
C(p) = [0 0 ... 0 -a0   ]
       [1 0 ... 0 -a1   ]
       [0 1 ... 0 -a2   ]
       [    ...         ]
       [0 0 ... 1 -a[n-1]]

where the coefficients a0 .. a[n-1] are from a monic polynomial
c0 + c1*t + c2*t^2 + ... c[n-1]*t^(n-1) + t^n

"""

def gen_companion_matrix(n):
    A = [[0 for i in range(n)] for j in range(n)]
    for i in range(n-1):
        A[i+1][i] = 1
    for i in range(n):
        A[i][n-1] = -1 * Symbol('a{}'.format(i))
    return Matrix(A)

"""

det(lambda*I - A) or det(lambda*I - transpose(A))
gives rise to the monic polynomial
c0 + c1*x + c2*x^2 ... c[n-1]*x^(n-1) + x^n
where lambda gets substituted to be x

"""

def test_monic_polynomial_property(n):
    l = Symbol('x')
    A = gen_companion_matrix(n)
    I = Matrix(np.identity(n))
    p = expand(simplify(det(l*I - A)))
    q = expand(simplify(det(l*I - A.T)))
    print("Order {}".format(n))
    pprint(p)
    pprint(q)
    print()

def test_power(n, m):
    l = Symbol('x')
    A = gen_companion_matrix(n)
    I = Matrix(np.identity(n))
    p = expand(simplify(det(l*I - A)))
    for i in range(m):
        print("Power {}x{} {}".format(n, n, i))
        pprint(A**i)

"""

The inverse of a companion matrix is in the form of
C(p)^-1 = [-a1/a0     1 0 ... 0
           -a2/a0     0 1 ... 0
           -a3/a0     0 0 ... 0
            ...
           -a[n-1]/a0 0 0 ... 1
           -1/a0]     0 0 ... 0]
"""
def test_inverse(n):
    l = Symbol('x')
    A = gen_companion_matrix(n)
    pprint(A)
    pprint(A.inv())
    pprint(A.T)
    pprint(A.T.inv())


init_printing()
print("Monic Polynomial Property")
for i in range(10):
    test_monic_polynomial_property(i)
print()

print("Power")
for i in range(10):
    test_power(i, 5)
print()

print("Inverse")
for i in range(10):
    test_inverse(i)
