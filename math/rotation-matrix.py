"""

https://optics.byu.edu/docs/opticsBook.pdf
http://scipp.ucsc.edu/~haber/ph116A/Rotation2.pdf
https://math.stackexchange.com/questions/3501114/understanding-a-consequence-of-eulers-rotation-theorem
https://en.wikipedia.org/wiki/Euler%27s_rotation_theorem
https://en.wikipedia.org/wiki/Rotation_formalisms_in_three_dimensions
https://en.wikipedia.org/wiki/Euler_angles
https://en.wikipedia.org/wiki/Euclidean_vector
https://en.wikipedia.org/wiki/Direction_cosine
https://d3cw3dd2w32x2b.cloudfront.net/wp-content/uploads/2012/07/euler-angles1.pdf
https://www.mathworks.com/help/aerotbx/ug/angle2dcm.html
https://en.wikipedia.org/wiki/Rodrigues'_rotation_formula
https://en.wikipedia.org/wiki/Axis%E2%80%93angle_representation
https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation
https://en.wikipedia.org/wiki/Singular_value_decomposition
https://scipp.ucsc.edu/~haber/ph116A/Rotation2.pdf
https://en.wikipedia.org/wiki/Eigenvalues_and_eigenvectors

"""

import math
import cmath
import numpy as np
import sympy
import sympy.abc
from scipy.stats import special_ortho_group
import sys

class Quat:
    def __init__(self, x, y, z, w):
        self.x = x
        self.y = y
        self.z = z
        self.w = w

    def __str__(self):
        return sympy.pretty([self.x, self.y, self.z, self.w])

    @staticmethod
    def from_axis_angle(axis, angle):
        s = sympy.sin(angle/2)
        c = sympy.cos(angle/2)
        q = Quat(axis[0]*s, axis[1]*s, axis[2]*s, c)
        return q

    def rotation_matrix(self):
        x = self.x
        y = self.y
        z = self.z
        w = self.w
        x2 = x*x
        y2 = y*y
        z2 = z*z
        xy = x*y
        xz = x*z
        yz = y*z
        wx = w*x
        wy = w*y
        wz = w*z
        s = 1
        return sympy.Matrix([[1 - 2*s*(y2+z2), 2*s*(xy-wz), 2*s*(xz+wy)],
                             [2*s*(xy+wz), 1 - 2*s*(x2+z2), 2*s*(yz-wx)],
                             [2*s*(xz-wy), 2*s*(yz+wx), 1 - 2*s*(x2+y2)]])


"""

The Euler rotation theorem states that for any rotation around an axis in 3D
(represented by a rotation matrix) there will be an axis that is unchanged by the rotation

Mathematically, this means the following:
R - rotation matrix
v - a vector that represents an axis of rotation
There is such a vector v where
R*v = R*R*v = R*R*R*v ...

To find that vector, we need to find the eigenvalues/eigenvectors of R
Since R is a NxN matrix where N is the dimension, we are going to get
N eigenvalues with their magnitude to be 1.

One of these eigenvalue will be (1+0i), and that corresponding eigenvector
will be the fixed axis of rotation because its eigenvalue is 1

ew = 1
R^n * ev = ew^n * ev  = ev

Generalizing this, for even dimensional space (2, 4, 6 ...),
there is no fixed axis of rotation because of the fundamental
theorem of algebra says that the complex roots always comes in pair,
so there is no eigenvalue of (1+0i) for those

The corollary is that in 3D, any number of rotations can be represented
by one rotation around a fixed axis

"""


def test_rotation_theorem(n):
    R = special_ortho_group.rvs(n)

    print("Rotation Matrix {}x{}".format(n, n))
    print(R)

    print("Eigenvalues and Eigenvectors")
    ew, ev = np.linalg.eig(R)
    vf = None
    for i in range(len(ew)):
        if abs(np.real(ew[i])-1) < 1e-6:
            vf = ev[:, i]
        print(ew[i])
        print(ev[:, i])
        print()

    print("Fixed axis of rotation")
    print(vf)
    if vf is not None:
        print(np.dot(R, vf[:]))
    print()


def make_matrix_from_angles(angle, axis, sym):
    if not sym:
        c = math.cos(angle)
        s = math.sin(angle)
    else:
        c = sympy.cos(angle)
        s = sympy.sin(angle)
    if axis == 'X':
        M = [[1, 0, 0], [0, c, -s], [0, s, c]]
    elif axis == 'Y':
        M = [[c, 0, s], [0, 1, 0], [-s, 0, c]]
    else:
        M = [[c, -s, 0], [s, c, 0], [0, 0, 1]]

    if sym:
        M = sympy.simplify(sympy.Matrix(M))

    return M


def extract_matrix_angle(M, conv):
    if conv == 'XYZ':
        t1 = math.atan2(M[1][2], M[2][2])
        c2 = math.sqrt(M[0][0]**2 + M[0][1]**2)
        t2 = math.atan2(-M[0][2], c2)
        s1 = math.sin(t1)
        c1 = math.cos(t1)
        t3 = math.atan2(s1*M[2][0] - c1*M[1][0], c1*M[1][1] - s1*M[2][1])
        t1, t2, t3 = -t1, -t2, -t3
    elif conv == 'ZYX':
        t1 = math.atan2(M[2][1], M[2][2])
        t2 = math.asin(-M[2][0])
        t3 = math.atan2(M[1][0], M[0][0])
    else:
        assert 0
    return (t1, t2, t3)


def angle2dcm(yaw, pitch, roll, conv='ZYX'):
    M = None
    if conv == 'RPY' or conv == 'ZYX':
        Z = make_matrix_from_angles(yaw, 'Z', False)
        Y = make_matrix_from_angles(pitch, 'Y', False)
        X = make_matrix_from_angles(roll, 'X', False)
        M = np.dot(Z, np.dot(Y, X))
    else:
        assert 0

    return np.linalg.inv(M)


def vector2dcm(E, N):
    n = E.shape[0]
    if n == 2:
        e1 = E[:, 0]
        e2 = E[:, 1]
        n1 = N[:, 0]
        n2 = N[:, 1]
        return np.array([
            [np.dot(n1, e1), np.dot(n1, e2)],
            [np.dot(n2, e1), np.dot(n2, e2)],
        ])
    elif n == 3:
        e1 = E[:, 0]
        e2 = E[:, 1]
        e3 = E[:, 2]
        n1 = N[:, 0]
        n2 = N[:, 1]
        n3 = N[:, 2]
        return np.array([
            [np.dot(n1, e1), np.dot(n1, e2), np.dot(n1, e3)],
            [np.dot(n2, e1), np.dot(n2, e2), np.dot(n2, e3)],
            [np.dot(n3, e1), np.dot(n3, e2), np.dot(n3, e3)],
        ])
    assert 0


def rodrigues_rotation_matrix(axis, angle):
    a1 = axis[0]
    a2 = axis[1]
    a3 = axis[2]
    K = sympy.Matrix([[0, -a3, a2], [a3, 0, -a1], [-a2, a1, 0]])
    I = sympy.eye(3)
    return sympy.trigsimp(sympy.expand(I + K*sympy.sin(angle) + np.dot(K, K)*(1-sympy.cos(angle))))

def sylvester(R, n):
    A = R[0][0]
    B = R[0][1]
    C = R[1][0]
    D = R[1][1]

    t = math.acos(0.5*(A + D))
    M = 1/np.sin(t) * np.array([[A*np.sin(n*t) - np.sin((n-1)*t), B*np.sin(n*t)], 
                                [C*np.sin(n*t),                   D*np.sin(n*t)-np.sin((n-1)*t)]])
    return M

"""

A frame of reference can be constructed from 3 angles conventionally denoted as

(alpha, beta, gamma) or
(psi, theta, phi)

There are 12 different sequence of rotation axes one can make with 3 angles
and are classified into 2 categories
Proper Euler angles (z-x-z, x-y-x, y-z-y, z-y-z, x-z-x, y-x-y)
Tait–Bryan angles   (x-y-z, y-z-x, z-x-y, x-z-y, z-y-x, y-x-z)

These represent rotation of an axis around the angle alpha, beta, or gamma
and we multiply all of those matrices generated together to get the concatenated matrix

These are active rotations, to find the passive rotations, transpose the matrix (which is the inverse)

"""


def test_matrix_angle_conventions():
    print("Matrix Angle Conventions")

    X1 = make_matrix_from_angles(sympy.abc.a, 'X', True)
    Y1 = make_matrix_from_angles(sympy.abc.a, 'Y', True)
    Z1 = make_matrix_from_angles(sympy.abc.a, 'Z', True)
    X2 = make_matrix_from_angles(sympy.abc.b, 'X', True)
    Y2 = make_matrix_from_angles(sympy.abc.b, 'Y', True)
    Z2 = make_matrix_from_angles(sympy.abc.b, 'Z', True)
    X3 = make_matrix_from_angles(sympy.abc.c, 'X', True)
    Y3 = make_matrix_from_angles(sympy.abc.c, 'Y', True)
    Z3 = make_matrix_from_angles(sympy.abc.c, 'Z', True)

    print()
    print("Proper Euler Angles")
    print()
    print("ZXZ")
    sympy.pprint(np.dot(X1, np.dot(Z2, X3)))

    print("XYX")
    sympy.pprint(np.dot(X1, np.dot(Y2, X3)))

    print("YXY")
    sympy.pprint(np.dot(Y1, np.dot(X2, Y3)))

    print("YZY")
    sympy.pprint(np.dot(Y1, np.dot(Z2, Y3)))

    print("ZYZ")
    sympy.pprint(np.dot(Z1, np.dot(Y2, Z3)))

    print("ZXZ")
    sympy.pprint(np.dot(Z1, np.dot(X2, Z3)))

    print()
    print("Tait-Bryan Angles")
    print()
    print("XZY")
    sympy.pprint(np.dot(X1, np.dot(Z2, Y3)))

    print("XYZ")
    sympy.pprint(np.dot(X1, np.dot(Y2, Z3)))

    print("YXZ")
    sympy.pprint(np.dot(Y1, np.dot(X2, Z3)))

    print("YZX")
    sympy.pprint(np.dot(Y1, np.dot(Z2, X3)))

    print("ZYX")
    sympy.pprint(np.dot(Z1, np.dot(Y2, X3)))

    print("ZXY")
    sympy.pprint(np.dot(Z1, np.dot(X2, Y3)))

    print()


"""

Given a rotation matrix, we want to extract
the 3 angles that make it up, the angles
returned depends on what convention we used
generally we return the angles in the [-pi, pi] range

"""


def test_matrix_angle_extraction():
    print("Matrix Angle Extraction")
    R1 = make_matrix_from_angles(0.3, 'X', False)
    R2 = make_matrix_from_angles(-0.5, 'Y', False)
    R3 = make_matrix_from_angles(0.6, 'Z', False)
    print(extract_matrix_angle(np.dot(R1, np.dot(R2, R3)), 'XYZ'))

    R1 = make_matrix_from_angles(-0.2, 'X', False)
    R2 = make_matrix_from_angles(-1.4, 'Y', False)
    R3 = make_matrix_from_angles(-math.pi, 'Z', False)
    print(extract_matrix_angle(np.dot(R1, np.dot(R2, R3)), 'XYZ'))

    R1 = make_matrix_from_angles(2.1, 'X', False)
    R2 = make_matrix_from_angles(-1.2, 'Y', False)
    R3 = make_matrix_from_angles(0.5, 'Z', False)
    print(extract_matrix_angle(np.dot(R1, np.dot(R2, R3)), 'XYZ'))

    R1 = make_matrix_from_angles(1.1, 'Z', False)
    R2 = make_matrix_from_angles(0.5, 'Y', False)
    R3 = make_matrix_from_angles(0.3, 'X', False)
    print(extract_matrix_angle(np.dot(R1, np.dot(R2, R3)), 'ZYX'))

    R1 = make_matrix_from_angles(-0.8, 'Z', False)
    R2 = make_matrix_from_angles(-1.2, 'Y', False)
    R3 = make_matrix_from_angles(1.3, 'X', False)
    print(extract_matrix_angle(np.dot(R1, np.dot(R2, R3)), 'ZYX'))

    print()


"""

A direction cosine matrix is making a rotation matrix
out of a given set of angles commonly referred to as (yaw, pitch, roll)

The direction cosine matrix name comes from constructing the cosine out of a set of vectors,
which when multiplied together can give us a rotation matrix

Given a vector P and Q, we want to find the angle between them:
dot(P, Q) = cos(angle) assuming P/Q are normalized which they usually are

Some common conventions:
RPY is used in airplanes which is the ZYX convention

Passive rotation is used to specify the rotation matrix
so need the inverse of the standard construction of the rotation matrix

"""


def test_angle2dcm():
    print("Direction Cosine Matrix from Angles")
    yaw = 0.7854
    pitch = 0.1
    roll = 0
    dcm = angle2dcm(yaw, pitch, roll)
    print(dcm)

    print(angle2dcm(0, 0, 0))


"""

If we are not given a set of angles to construct a direction cosine matrix
but two sets of orthogonal unit basis vectors
(e1, e2)     (n1, n2)     2D
(e1, e2, e3) (n1, n2, n3) 3D

We want to construct a matrix that transforms vectors from basis E to N.
We can construct a direction cosine matrix A as follows:
A_ij = dot(n_i, e_j)

This matrix that we constructed from this procedure is equivalent to the following transformation:

E = orientation matrix of basis vectors e_i
N = orientation matrix of basis vectors n_i
A = N^-1*E

"""


def test_vector2dcm(n):
    E = special_ortho_group.rvs(n)
    N = special_ortho_group.rvs(n)
    I = np.eye(n)

    EN = vector2dcm(E, N)
    NE = vector2dcm(N, E)

    print("Direction Cosine Matrix from Vectors")

    print("The basis matrix")
    print(E)
    print()
    print(N)
    print()

    # Transforming to the same basis gives us the identity matrix as the transformation matrix
    print("Transforming to the same Basis")
    print(vector2dcm(E, E))
    print(vector2dcm(N, N))
    print()

    # The basis transformation E -> N is the inverse of N <- E and vice versa
    # Since this is a orthonormal matrix, they are transpose of each others
    print("From <-> To")
    print(EN)
    print(np.dot(np.linalg.inv(N), E))
    print()
    print(NE)
    print(np.dot(np.linalg.inv(E), N))
    print()

    # Transforming from the standard basis I -> N gives us the transformation N^-1
    print("Identity -> To")
    print(np.linalg.inv(E))
    print(vector2dcm(I, E))
    print()
    print(np.linalg.inv(N))
    print(vector2dcm(I, N))
    print()


"""

If we want to rotate around an arbitrary axis instead of a product of rotations around the standard axes, we can use the rodrigues rotation formula.

The matrix form of the formula can be derived using the exponential mapping

R = exp(K*theta)
K = w x v where w is the unit vector and v is the vector to be rotated
K is written out as a cross product matrix

We can expand the matrix exponential as a taylor series
I + theta*K + (theta*K)^2/2! + (theta*K)^3/3! ...
Because K is skew-symmetric and the sum of squares of the above diagonal is 1, the characteristic polynomial
implies that K^3 = -K, K^4 = -K^2 ...
So we can group these together and simplify to get

R = I + sin(theta)*K + (1-cos(theta))*K^2

This is also known as the formula to transform SU(2) to SO(3).
SU(2) double covers SO(3) and does not have gimbal locks and is isomorphic to the quaternions.

"""


def test_rodrigues_formula():
    print()
    print("Rodrigues Rotation Formula")
    # gives the standard euler rotation matrix around X, Y, Z axes
    print("\nX Axis\n")
    sympy.pprint(rodrigues_rotation_matrix([1, 0, 0], sympy.abc.theta))
    print("\nY Axis\n")
    sympy.pprint(rodrigues_rotation_matrix([0, 1, 0], sympy.abc.theta))
    print("\nZ Axis\n")
    sympy.pprint(rodrigues_rotation_matrix([0, 0, 1], sympy.abc.theta))

    # general formula for rotation around arbitrary axis
    print("\nArbitrary Axis\n")
    sympy.pprint(rodrigues_rotation_matrix(
        [sympy.abc.x, sympy.abc.y, sympy.abc.z], sympy.abc.theta))


"""

Quaternions can be used as an alternative to Euler angles, using standard quaternion construction formulas
we can show that it gives us the same formula as the rotation matrices generated from Euler angles

"""


def test_quaternion():
    # gives standard euler rotation matrix around X, Y, Z axes
    X = Quat.from_axis_angle([1, 0, 0], sympy.abc.theta)
    Y = Quat.from_axis_angle([0, 1, 0], sympy.abc.theta)
    Z = Quat.from_axis_angle([0, 0, 1], sympy.abc.theta)
    R = Quat.from_axis_angle(
        [sympy.abc.x, sympy.abc.y, sympy.abc.z], sympy.abc.theta)
    print("\nQuaternion\n")
    print("\nX Axis\n")
    sympy.pprint(sympy.simplify(X.rotation_matrix()))
    print("\nY Axis\n")
    sympy.pprint(sympy.simplify(Y.rotation_matrix()))
    print("\nZ Axis\n")
    sympy.pprint(sympy.simplify(Z.rotation_matrix()))

    # this gives the same formula as the rodrigues formula
    print("\nArbitrary Axis\n")
    s = R.rotation_matrix()
    s = s.subs(sympy.sin(sympy.abc.theta/2)**2,
               (1-sympy.cos(sympy.abc.theta))/2)
    s = sympy.expand(sympy.simplify(s))
    sympy.pprint(s)

"""

If a determinant of a 2x2 matrix is 1 (a rotation matrix) then sylvester theorem says
that there is a closed form formula for raising that matrix to a integer power.

R = [[A B]  
     [C D]]

R^N is then equal to the following matrix M

t = math.acos(0.5*(A + D))
M = 1/sin(t) * [[A*sin(n*t) - sin((n-1)*t), B*sin(n*t)], 
                [C*sin(n*t),                D*sin(n*t)-sin((n-1)*t)]])

"""
def test_sylvester_theorem():
    R = special_ortho_group.rvs(2)
    X = R.copy()
    for i in range(10):
        S = sylvester(X, i+1)
        assert(np.allclose(R, S))

        R = np.dot(R, X)

"""

For a given rotation matrix, SVD can decompose the matrix into the following:

R = U * S * V Where U and V are rotation matrices and S is a diagonal matrix (scale in this case)

The decomposition gives the following dimension for the matrix

R = MxM matrix
U = MxM complex unitary matrix
S = MxN rectangular diagonal matrix with non-negative numbers on the diagonal
V = NxN complex unitary matrix


Since a rotation matrix has a determinant 1, S will be 1s also so
So the equation becomes

R = U * V
R^T = V^T * U^T
transpose(R) = transpose(V)*transpose(U)

"""

def test_svd():
    for d in range(2, 10):
        for i in range(1000):
            R = special_ortho_group.rvs(d)
            U, S, V = np.linalg.svd(R)
            assert(np.allclose(S*np.eye(d), np.eye(d)))
            assert(np.allclose(R, np.dot(U, V)))
            assert(np.allclose(np.transpose(R), np.dot(np.transpose(V), np.transpose(U))))

"""

Given a 2D rotation matrix:
[cos(t) -sin(t)]
[sin(t) cos(t)]

Eigenvalue/Eigenvectors:

e1 = exp(i*t)
e2 = exp(-i*t)

v1 = [1 -i]
v2 = [1 i]

"""

def test_eigen_2d():
    t = 0
    while t <= 2*math.pi:
        R = np.array([[math.cos(t), -math.sin(t)], [math.sin(t), math.cos(t)]])
        v1 = np.array([1, -1j])
        v2 = np.array([1, 1j])
        e1 = cmath.exp(1j * t)
        e2 = cmath.exp(-1j * t)

        for i in range(10):
            rv1 = np.dot(R, v1)
            rv2 = np.dot(R, v2)
            
            xv1 = e1 * v1
            xv2 = e2 * v2

            v1 = rv1
            v2 = rv2
            
            np.allclose(rv1, xv1)
            np.allclose(rv2, xv2)

        t += 1e-3

sympy.init_printing()

for i in range(2, 6):
    test_rotation_theorem(i)
test_matrix_angle_conventions()
test_matrix_angle_extraction()
test_angle2dcm()
test_vector2dcm(2)
test_vector2dcm(3)
test_rodrigues_formula()
test_quaternion()
test_sylvester_theorem()
test_svd()
test_eigen_2d()
