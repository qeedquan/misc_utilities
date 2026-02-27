import numpy as np
from sympy import *

"""

The transfer function is defined as

Y(s)/U(s) = G(s)

The state space equations is defined as
x' = Ax + Bu
y  = Cx + du

Take the laplace transform of the state space equation and get
sX(s) - x(0) = AX(s) + BU(s)
Y(s)         = CX(s) + DU(s)

The Laplace Transform assumes the initial conditions to be 0 so x(0) = 0
sX(s) - AX(s) = BU(s)

Factor X(s) out
(sI - A)X(s) = B*U(s)

Solve for X(s)
X(s) = (sI - A)^-1BU(s)

Substitute X(s)
Y(s) = CX(s) + DU(s)
Y(s) = (C(sI - A)^-1B + D)U(s)

We can see that the equation
G(s) = C(sI - A)^-1B + D

"""

def st2tf(A, B, C, D):
    s = Symbol('s')
    I = eye(A.shape[0])
    G = C*(s*I - A)**-1*B + D
    G = simplify(G)
    
    print('Converting State Space Matrix to Transfer Function')
    print('\nA\n\n')
    pprint(A)
    print('\nB\n\n')
    pprint(B)
    print('\nC\n\n')
    pprint(C)
    print('\nD\n\n')
    pprint(D)
    print('\nG\n\n')
    pprint(G)
    print('\n\n')

b = Symbol('b')
k = Symbol('k')
m = Symbol('m')
Kp = Symbol('kp')
taup = Symbol('taup')
tau = Symbol('tau')
zeta = Symbol('zeta')

A = Matrix([[0, 1], [-k/m, -b/m]])
B = Matrix([[0], [1/m]])
C = Matrix([[1, 0]])
D = Matrix([0])
st2tf(A, B, C, D)

A = Matrix([[-14, -56, -160], [1, 0, 0], [0, 1, 0]])
B = Matrix([[1], [0], [0]])
C = Matrix([[0, 1, 0]])
D = Matrix([0])
st2tf(A, B, C, D)

A = Matrix([-1/taup])
B = Matrix([Kp/taup])
C = Matrix([1])
D = Matrix([0])
st2tf(A, B, C, D)

A = Matrix([[0,1],[-1/tau**2,-2*zeta/tau]])
B = Matrix([[0],[Kp/tau**2]])
C = Matrix([[1, 0]])
D = Matrix([0])
st2tf(A, B, C, D)

A = Matrix([[0, 1], [0, -10/100]])
B = Matrix([[0], [1/100]])
C = Matrix([[1, 0]])
D = Matrix([0])
K = Matrix([[30, 70]])
st2tf(A, B, C, D)
st2tf(A - np.dot(B, K), B, C, D)
