"""

https://en.wikipedia.org/wiki/Pauli_matrices
https://physics.stackexchange.com/questions/271419/why-is-there-this-relationship-between-quaternions-and-pauli-matrices

"""

from sympy.abc import *
from sympy import *
import numpy as np

def pauli_matrices():
    u = Matrix([[1, 0], [0, 1]])
    o1 = Matrix([[0, 1], [1, 0]])
    o2 = Matrix([[0, -I], [I, 0]])
    o3 = Matrix([[1, 0], [0, -1]])
    return (u, o1, o2, o3)

"""

Quaternion basis elements can be constructed from
2x2 pauli matrices (kind of like how complex numbers can be represented by a 2x2 matrix)
by the following

1 = identity(2)
i = o1*-I
j = o2*-I
k = o2*-I

then we can verify that it this matrix construction satisfy all properties of a quaternion

"""
def test_quaternion_property():
    u, o1, o2, o3 = pauli_matrices()
    i = -I*o1
    j = -I*o2
    k = -I*o3

    print("Quaternion Property")
    print("1")
    pprint(u)
    print("i")
    pprint(i)
    print("j")
    pprint(j)
    print("k")
    pprint(k)
    print("\n")
    
    print("i^2 = j^2 = k^2 = -1")
    pprint(i@i)
    pprint(j@j)
    pprint(k@k)
    pprint(-1*u)
    print()

    print("i*j=k")
    pprint(i@j)
    print("j*k=i")
    pprint(j@k)
    print("k*i=j")
    pprint(k@i)
    print("-i*k=j")
    pprint(-i@k)

    print("conjugate")
    Q = u+i+j+k
    Qc = u-i-j-k
    pprint(Q)
    pprint(Qc)
    pprint(simplify(Q@Qc))

"""

The commutator relation is defined on two operator A and B
[A, B] = A*B - B*A
[A, B] = 0 when the two operators are commutative, non-zero otherwise

The anticommutator relation is defined as
{A, B} = A*B + B*A

The pauli matrices can represent a linear operator in this case so we can
apply the commutator/anticommutator relations on them.

In general, pauli matrices have the following commutator/anticommutator relations
[o_a, o_b] = 2*i*e_abc*o_c
{o_a, o_b} = 2*s_ab*I
e_abc is the structure constant in Levi-Cevita symbol
s_ab is the kronecker delta
I is the 2x2 identity matrix

"""

def test_commutation():
    u, o1, o2, o3 = pauli_matrices()
    print("Commutation Properties")
    print("[o1, o2] = 2*I*o3")
    pprint(o1*o2 - o2*o1)
    pprint(2*I*o3)
    print("[o2, o3] = 2*I*o1")
    pprint(o2*o3 - o3*o2)
    pprint(2*I*o1)
    print("[o3, o1] = 2*I*o2")
    pprint(o3*o1 - o1*o3)
    pprint(2*I*o2)
    print("[o1, o1] [o2, o2] [o3, o3] = 0")
    pprint(o1*o1 - o1*o1)
    pprint(o2*o2 - o2*o2)
    pprint(o3*o3 - o3*o3)
    print("Anticommutator Properties")
    print("{o1, o1} = 2*I")
    pprint(o1*o1 + o1*o1)
    pprint(2*u)
    pprint("{o1, o2} = 0")
    pprint(o1*o2 + o2*o1)
    pprint("{o1, o3} = 0")
    pprint(o1*o3 + o3*o1)

"""

The trace properties for the Pauli matrices:

tr(o_a) = 0
tr(o_a*o_b) = 2*s_ab
tr(o_a*o_b*o_c) = 2*i*e_abc
tr(o_a*o_b*o_c*o_d) = 2*(o_ab*o_cd - o_ac*o_bd + o_ad*o_bc)

"""


def test_trace():
    u, o1, o2, o3 = pauli_matrices()
    print("Trace Properties")
    print("tr(o1) tr(o2) tr(o3) = 0")
    print(np.trace(o1), np.trace(o2), np.trace(o3))
    print("tr(o_a*o_b) = 2*s_ab")
    print(np.trace(o1@o1), np.trace(o2@o2), np.trace(o3@o3))
    print(np.trace(o1@o3), np.trace(o2@o1), np.trace(o3@o2))
    print(np.trace(o1@o1@o1@o1))

init_printing()
test_quaternion_property()
test_commutation()
test_trace()
