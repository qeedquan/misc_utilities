from sympy import *
from sympy.physics.vector import *
from sympy.vector import *

R = CoordSys3D('R')
t = Symbol('t')
a = -sin(t)*R.i + cos(t)*R.j + 0*R.k
b = -cos(t)*R.i + -sin(t)*R.j + 0*R.k

# same
print(simplify(dot(a, b)))
print(simplify(a & b))

# same
print(simplify(a ^ b))
print(simplify(cross(a, b)))

