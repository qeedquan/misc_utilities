"""

http://cs-people.bu.edu/sbargal/Fall%202016/lecture_notes/Nov_3_3d_geometry_representation
https://en.wikipedia.org/wiki/Quadratic_form
https://en.wikipedia.org/wiki/Conic_section
https://en.wikipedia.org/wiki/Matrix_representation_of_conic_sections

"""

import numpy as np
from sympy import *

def print_implicit(title, Q, p, eig=True):
    M = Matrix(Q)
    v = expand(np.dot(p.T, np.dot(Q, p)))
    n = 2 * np.dot(Q, p)
    print(title)
    print("Matrix")
    pprint(M)
    print("Implicit")
    print(v)
    print("Normal")
    print(n[:len(n)-1])
    print("Determinant")
    print(M.det())
    if eig:
        print("Eigenvalues and Eigenvectors")
        pprint(M.eigenvals())
        pprint(M.eigenvects())
    print()


"""

Conic sections can be viewed as a 2D quadric surface and can be represented by the 3x3 symmetric matrix
M = [A   B/2 D/2
     B/2 C   E/2
     D/2 E/2 F]

where A-F are constants

To test if a point lies on the conic section we can do
p = [x y 1] (homogeneous point)
transpose(p) * M * p = 0

If we symbolically multiply the equation above out, we get 
A*x**2 + B*x*y + C*y**2 + D*x + E*y + F = 0

The determinant of the matrix M gives us what "shape" of the solution set is
det(M) < 0 gives ellipse (If A=C, B=0), gives us a circle
det(M) = 0 gives parabola
det(M) > 0 gives hyperbola (if A+C = 0), gives us a rectangular hyperbola

Circle
M = [1 0 0
     0 1 0
     0 0 -r^2]

x^2 + y^2 - r^2

Ellipse
M = [1/a^2 0 0
     0 1/b^2 0
     0 0 -1]

x^2/a^2 + y^2/b^2 - 1

Parabola
M = [4a/x 0 0
     0 -1 0
     0 0 0]

y^2 = 4ax

Hyperbola
M = [1/a^2 0 0
     0 -1/b^2 0
     0 0 -1]

x^2/a^2 - y^2/b^2 = 1

"""
def test_implicit_matrix_form_2d():
    A = Symbol('A')
    B = Symbol('B')
    C = Symbol('C')
    D = Symbol('D')
    E = Symbol('E')
    F = Symbol('F')

    x = Symbol('x')
    y = Symbol('y')
    r = Symbol('r')
    a = Symbol('a')
    b = Symbol('b')
    p = np.array([x, y, 1])

    print("Conic Sections Implicit Matrix Form")

    Q = np.array([[A, B/2, D/2], [B/2, C, E/2], [D/2, E/2, F]])
    print_implicit("General", Q, p, eig=False)

    Q = np.array([[1, 0, 0], [0, 1, 0], [0, 0, -(r*r)]])
    print_implicit("Circle", Q, p)

    Q = np.array([[1/(a*a), 0, 0], [0, 1/(b*b), 0], [0, 0, -1]])
    print_implicit("Ellipse", Q, p)

    Q = np.array([[1/(a*a), 0, 0], [0, -1/(b*b), 0], [0, 0, -1]])
    print_implicit("Hyperbola", Q, p)

    Q = np.array([[4*a/x, 0, 0], [0, -1, 0], [0, 0, 0]])
    print_implicit("Parabola", Q, p)

"""

Implicit equations that describe quadric surfaces can be represented as a 4x4 symmetric matrix
M = [A B C D
     B E F G
     C F H I
     D G I J]

where A-J are constants

To test whether a point (represented in homogenous form)
p = [x y z 1] lies on the quadric surface we have

tranpose(p) * M * p = 0

If we symbolically do the matrix multiplication above, we get this equation:
A*x**2 + 2*B*x*y + 2*C*x*z + 2*D*x + E*y**2 + 2*F*y*z + 2*G*y + H*z**2 + J + 2*I*z = 0

In a sense, M is a matrix of a quadratic form
transpose(x)*A*x + transpose(b)*x = c
and transpose(b)*x and c is 0 in this case
The eigenvalues of A determine the "shape" solution set, such as a ellipsoid or hyperboloid

If all eigenvalues of A are positive, then it is a ellipsoid
If all eigenvalues of A are negative, then it is a imginary ellipsoid
If some eigenvalues are positive and some negative, it is a hyperboloid
If one or more eigenvalues is zero, then the shape depends on the b vector, then it can be elliptic/hyperbolic paraboloid

The normal can be calculated by taking the gradient of equation
gradient(transpose(p) * M * p) = 2*Q*p
This gives us the normal vector for every point that is determined to be on the surface by the implicit form

Depending what we set the constants A-J as, we can get a sphere, hyperbolas, ellipsoid, cones

Sphere
M = [1 0 0 0
     0 1 0 0
     0 0 1 0
     0 0 0 -r^2]

N = [2x 2y 2z]

x^2 + y^2 + z^2 - r^2

Ellipsoid
M = [1/rx^2 0 0 0
     0 1/ry^2 0 0
     0 0 1/rz^2 0
     0 0 0 -1]

N = [2*x/rx^2 2*y/ry^2 2*z/rz^2]

x^2/rx^2 + y^2/ry^2 + z^2/rz^2 - 1

Cylinder
M = [1/rx^2 0 0 0
     0 1/ry^2 0 0
     0 0 0 0
     0 0 0 -1]

N = [2*x/rx^2 2*y/ry^2 0]

x^2/rx^2 + y^2/ry^2 - 1

Cone
M = [1/rx^2 0 0 0
     0 1/ry^2 0 0
     0 0 -1/s^2 0
     0 0 0 0]

N = [2*x/rx^2 2*y/ry^2 -2*z/s^2]

x^2/rx^2 + y^2/ry^2 - z^2/s^2

Plane (Not a quadric surface but it can be represented in this form)
M = [0 0 0 a/2
     0 0 0 b/2
     0 0 0 c/2
     a/2 b/2 c/2 -d]

N = [a b c]

ax + by + cz - d

"""
def test_implicit_matrix_form_3d():
    A = Symbol('A')
    B = Symbol('B')
    C = Symbol('C')
    D = Symbol('D')
    E = Symbol('E')
    F = Symbol('F')
    G = Symbol('G')
    H = Symbol('H')
    J = Symbol('J')
   
    a = Symbol('a')
    b = Symbol('b')
    c = Symbol('c')
    d = Symbol('d')
    r = Symbol('r')
    s = Symbol('s')
    rx = Symbol('rx')
    ry = Symbol('ry')
    rz = Symbol('rz')

    x = Symbol('x')
    y = Symbol('y')
    z = Symbol('z')
    s = Symbol('s')
    p = np.array([x, y, z, 1])

    print("Quadric Surfaces Implicit Matrix Form")
    print()
    
    Q = np.array([[A, B, C, D], [B, E, F, G], [C, F, H, I], [D, G, I, J]])
    print_implicit("General", Q, p, eig=False)

    Q = np.array([[1, 0, 0, 0], [0, 1, 0, 0], [0, 0, 1, 0], [0, 0, 0, -(r*r)]])
    print_implicit("Sphere", Q, p)

    Q = np.array([[1/(rx*rx), 0, 0, 0], [0, 1/(ry*ry), 0, 0], [0, 0, 1/(rz*rz), 0], [0, 0, 0, -1]])
    print_implicit("Ellipsoid", Q, p)

    Q = np.array([[1/(rx*rx), 0, 0, 0], [0, 1/(ry*ry), 0, 0], [0, 0, 0, 0], [0, 0, 0, -1]])
    print_implicit("Cylinder", Q, p)

    Q = np.array([[1/(rx*rx), 0, 0, 0], [0, 1/(ry*ry), 0, 0], [0, 0, -1/(s*s), 0], [0, 0, 0, 0]])
    print_implicit("Cone", Q, p)

    Q = np.array([[0, 0, 0, a/2], [0, 0, 0, b/2], [0, 0, 0, c/2], [a/2, b/2, c/2, -d]])
    print_implicit("Plane", Q, p)

init_printing()
test_implicit_matrix_form_2d()
print("")
test_implicit_matrix_form_3d()
