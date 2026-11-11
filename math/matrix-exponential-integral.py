"""

https://en.wikipedia.org/wiki/Matrix_exponential
https://math.stackexchange.com/questions/658276/integral-of-matrix-exponential

"""

import numpy as np
from scipy.linalg import *

# if A is nonsingular then
# integrate expm(tA) from 0 to T = expm(t*A - I)*A^-1
def expmint1(A, T):
    return np.dot(expm(T*A) - np.identity(len(A)), inv(A))

def expmint2(A, T, nbins=100):
    f = lambda x: expm(A*x)
    xv = np.linspace(0,T,nbins)
    result = np.apply_along_axis(f,0,xv.reshape(1,-1))
    return np.trapz(result,xv)

def expmint3(A, t, nbins=50):
    taylor = t*np.array([np.linalg.matrix_power(A*t,k)/np.math.factorial(k+1) for k in range(nbins)])
    integral = taylor.sum(axis = 0)
    return integral

def test(A, t0, t1, dt):
    # det(e^A) = e^(tr(A))
    print("Det", det(expm(A)), np.exp(np.trace(A)))
    t = t0
    while t <= t1:
        print(t)
        print(expmint1(A, t))
        print(expmint2(A, t))
        print(expmint3(A, t))
        print()
        t += dt
    print()

A = np.array([[0.1, 0.3, 3], [0.3, 0.4, 0.3], [0.3, 0.1, 0.3]])
B = np.array([[1, 2.3, 3], [0.3, 0.4, 0.3], [0.3, 0.1, 0.3]])
C = np.random.rand(5, 5)
test(A, 0, 10, 0.5)
test(B, -5, 3, 1)
test(C, 0, 5, 0.3)
