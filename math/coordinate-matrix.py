"""

https://en.wikipedia.org/wiki/Orthogonal_coordinates
https://en.wikipedia.org/wiki/Covariance_and_contravariance_of_vectors
https://en.wikipedia.org/wiki/Log-polar_coordinates
https://en.wikipedia.org/wiki/Parabolic_coordinates
https://en.wikipedia.org/wiki/Jacobian_matrix_and_determinant
https://archive.lib.msu.edu/crcmath/math/math/p/p055.htm
https://en.wikiversity.org/wiki/PlanetPhysics/Transformation_Between_Cartesian_Coordinates_and_Polar_Coordinates
https://en.wikipedia.org/wiki/Elliptic_coordinate_system
https://en.wikipedia.org/wiki/Bipolar_coordinates
https://www.researchgate.net/publication/309803947_A_Note_on_Elliptic_Coordinates

"""

from sympy import *
from sympy.abc import *

"""

The matrix for the coordinate transform are not unique, we can parameterize it in some other way.
Depending on the parameterization, the determinants are different even though the matrix transform gives the same formula at the end.
If we want consistency, we like the transformation to and from to preserve the determinant, though the determinant can be 0 for the most part,
since in a different coordinate system orientation/volumes are usually not preserved.

These coordinate transformation from one coordinate to another are not constant values, so writing it as a matrix doesn't have advantages.
If the values were constant we can think of them as a transformation in that coordinate system rather than going to a different coordinate system.

Cartesian <-> Polar
r = sqrt(x^2 + y^2)
t = atan2(y, x)

x = r*cos(t)
y = r*sin(t)

Cartesian <-> Log Polar
p = log(sqrt(x^2 + y^2))
t = atan2(y, x)

x = e^p*cos(t)
y = e^p*sin(t)

Cartesian <-> Parabolic
x = s*t
y = 0.5*(s*s - t*t)

s = sqrt(sqrt(x^2 + y^2) + y)
t = sqrt(sqrt(x^2 + y^2) - y)

Cartesian <-> Elliptic

Let c be a fixed constant
u >= 0
0 <= v <= 2*pi
x = c*cosh(u)*sinh(v)
y = c*sinh(u)*sin(v)

B = x^2 + y^2 - c^2
d = sqrt(B^2 + 4*c^2*y^2)
e = 2*c^2
p = (-B+d)/e
q = (-B-d)/e
u = 0.5*ln(1 - 2*q + 2*sqrt(q^2 - q))

v0 = asin(sqrt(p))
v = v0        [x >= 0, y >= 0]
v = pi - v0   [x < 0, y >= 0]
v = pi + v0   [x <= 0, y < 0]
v = 2*pi - v0 [x > 0, y < 0]

"""
def test_coordinate_matrix():
    print("Coordinate Transformation Matrix\n")
    
    l = 0.5*sqrt(x**2 + y**2)
    t = 0.5*atan(y/x)
    M = Matrix([[l/x, l/y], [t/x, t/y]])
    v = Matrix([x, y])
    print("Cartesian to Polar det(M)={}".format(M.det()))
    pprint(simplify(M))
    pprint(simplify(M@v))
    print("\n")

    M = Matrix([[cos(theta), 0], [sin(theta), 0]])
    v = Matrix([r, t])
    print("Polar to Cartesian det(M)={}.".format(M.det()))
    pprint(simplify(M))
    pprint(simplify(M@v))
    print("\n")

    l = log(sqrt(x**2 + y**2))/2
    M = Matrix([[l/x, l/y], [t/x, t/y]])
    v = Matrix([x, y])
    print("Cartesian to Log Polar det(M)={}".format(M.det()))
    pprint(simplify(M))
    pprint(simplify(M@v))
    print("\n")

    M = Matrix([[exp(phi)*cos(theta)/phi, 0], [exp(phi)*sin(theta)/phi, 0]])
    v = Matrix([phi, theta])
    print("Log Polar to Cartesian det(M)={}".format(M.det()))
    pprint(simplify(M))
    pprint(simplify(M@v))
    print("\n")

    a = 0.5*sigma*tau
    b = 0.25*(sigma**2 - tau**2)
    M = Matrix([[a/sigma, a/tau], [b/sigma, b/tau]])
    v = Matrix([sigma, tau])
    print("Parabolic to Cartesian det(M)={}".format(M.det()))
    pprint(simplify(M))
    pprint(simplify(M@v))
    print("\n")

    print("Cartesian to Parabolic det(M)={}".format(M.det()))
    a = sqrt(x**2 + y**2)
    b = 0.5*sqrt(a + y)
    c = 0.5*sqrt(a - y)
    M = Matrix([[b/x, b/y], [c/x, c/y]])
    v = Matrix([x, y])
    pprint(simplify(M))
    pprint(simplify(M@v))
    print("\n")

"""

Given a coordinate system, we want to generate a transformation matrix that transforms a covariant vector
The general matrix can be generated using the following method:

Given a coordinate system of N variables, we want to transform it into another system of N variables, hence
we will have a NxN matrix we shall call A

Aij = partial derivative of the ith variable in the second coordinate system with respect to the jth derivative in the first derivative

For example, for polar to cartesian we have two variables:
(r, theta) -> (x, y)

A11 = (x, r)
A12 = (x, theta)
A21 = (y, r)
A21 = (y, theta)

We calculate the partial derivative of those variables and fill the matrix, in general they are also not constant values but some function

If they were just constant values, the matrix transform will be just be inverse of that matrix.
This matrix is also known as the jacobian matrix and its determinant is known as the jacobian determinant.

"""

def test_contravariant_matrix():
    print("Contravariant Vector Transformation Matrix\n")

    l = sqrt(x**2 + y**2)
    M = Matrix([[x/l, y/l], [-y/l**2, x/l**2]])
    v = Matrix([x, y])
    print("Cartesian to Polar det(M)={}".format(M.det()))
    pprint(simplify(M))
    pprint(simplify(M@v))
    print("")
    
    print("Polar to Cartesian det(M)={}".format(M.det()))
    M = Matrix([[cos(theta), -r*sin(theta)], [sin(theta), r*cos(theta)]])
    v = Matrix([r, theta])
    pprint(simplify(M))
    pprint(simplify(M@v))
    print("")

test_coordinate_matrix()
test_contravariant_matrix()
