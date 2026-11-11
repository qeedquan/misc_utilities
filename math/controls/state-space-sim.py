"""

https://en.wikipedia.org/wiki/State-space_representation
https://www.kalmanfilter.net/modeling.html

"""

import numpy as np
from scipy.linalg import *

def expmint(A, t, nbins=100):
    f = lambda x: expm(A*x)
    xv = np.linspace(0, t, nbins)
    result = np.apply_along_axis(f, 0, xv.reshape(1,-1))
    return np.trapz(result,xv)

"""

State space equation can be written as

x'(t) = A*x(t) + B*u(t)
y(t)  = C*x(t) + D*u(t)

integrate x'(t) to get x(t)
F = expm(A*dt)
G = integrate(expm(A*dt) * B, 0, t)
x(t + dt) = F*x(t) + G*u(t)

Set t = 0, so dt becomes the time itself

y(t) = C*(F*x(t) + G*u(t)) + D*u(t)

The A matrix can be written
[0 1 0 ... 0 0]
[0 0 1 ... 0 0]
[      ...    ]
[0 0 0 ... 0 1]
[-a0/an -a1/an ... -a_n-1/an]

The B matrix can be written as
[0]
[0]
...
[0]
[1/an]

In general
x(t) exists in R^n
y(t) exists in R^q
u(t) exists in R^p
A is NxN matrix
B is NxP matrix
C is QxN matrix
D can be 0x0 matrix if no feedthrough, or a QxP matrix
x'(t) is d/dt(x(t))

"""

def sseval(A, B, C, D, x, u, t):
    F = expm(A*t)
    G = np.dot(expmint(A, t), B)
    xn = np.dot(F, x) + np.dot(G, u)
    yn = np.dot(C, xn) + np.dot(D, u)
    return xn, yn

def sim(A, B, C, D, x, u, f, t0, t1, dt):
    print('A {} det(A) {}'.format(A.shape, det(A)))
    print(A)
    print('\nB', B.shape)
    print(B)
    print('\nC', C.shape)
    print(C)
    print('\nD', D.shape)
    print(D)
    print('\nx', x.shape)
    print(x)
    print('\nu', u.shape)
    print(u)
    print()

    t = t0
    while t <= t1:
        _, y = sseval(A, B, C, D, x, u, t)
        
        v0 = y
        v1 = f(t)

        print("{:.3f}".format(t))
        print(v0)
        print(v1)
        print()
        
        t += dt
    
    print('-'*32)

def test_constant_velocity():
    A = np.array([[0, 1], [0, 0]])
    B = np.array([[0], [0]])
    C = np.array([[1, 0]])
    D = np.array([[0]])

    p0, v0 = 50, 0.34
    p1, v1 = -10, 5.31
    p2, v2 = -354, -301

    x = np.array([[p0], [v0]])
    u = np.array([[0]])
    f = lambda t: x[0] + x[1]*t 
    sim(A, B, C, D, x, u, f, 0, 10, 1)

    x = np.array([[p1], [v1]])
    sim(A, B, C, D, x, u, f, 0, 6, 1)

    x = np.array([[p2], [v2]])
    sim(A, B, C, D, x, u, f, 10, 12.5, 0.35)

def test_constant_acceleration():
    A = np.array([[0, 1], [0, 0]])
    B = np.array([[0], [1]])
    C = np.array([[1, 0], [0, 1]])
    D = np.array([[0]])

    p0, v0, a0 = 41, 304, 10.5
    p1, v1, a1 = -45, 100, -9.8
    
    x = np.array([[p0], [v0]])
    u = np.array([[a0]])
    f = lambda t: np.array([x[0] + x[1]*t + u[0]*t*t*0.5, x[1] + u[0]*t])
    sim(A, B, C, D, x, u, f, 0, 10, 1)

    x = np.array([[p1], [v1]])
    u = np.array([[a1]])
    sim(A, B, C, D, x, u, f, -30, 30, 3.56)

# Verify the state space evolution is equal to the equations if we setup the matrices properly
np.set_printoptions(precision=3, suppress=True)
test_constant_velocity()
test_constant_acceleration()
