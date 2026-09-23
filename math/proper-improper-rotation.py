"""

https://en.wikipedia.org/wiki/Pseudovector
https://en.wikipedia.org/wiki/Improper_rotation

"""

import numpy as np
from scipy.stats import special_ortho_group

def rotate_vectors(R, v1, v2, v3, muldet):
    d = np.linalg.det(R)
    v1p = np.dot(R, v1)
    v2p = np.dot(R, v2)
    v3p = np.dot(R, v3)
    if muldet:
        v3p *= d
    vcp = np.cross(v1p, v2p)
    return (d, v1p, v2p, v3p, vcp)

"""

A pseudovector (axial vector) is a quantity that transforms like a vector
under proper rotation but under improper rotation, the sign flips

An improper rotation is a reflection/mirror, mathematically it is a
rotation matrix but has a determinant of -1

A cross product of 2 vectors cross(a, b) gives a pseudovector c and a and b are
considered to be polar vectors

Putting the above together, this means that if we have two vectors a and b
and we take the cross product of them like so:
c = cross(a, b)
a' = R*a
b' = R*b
c' = R*c

if R was a proper rotation matrix (det(R)=1) then
cross(a', b') = c'

however if R was a improper rotation matrix (det(R)=-1) then
cross(a', b') = -c'

The way to fix this is to account for the determinant of the rotation matrix for the pseudovector
a' = R*a
b' = R*b
c' = det(R)*R*c

Here cross(a', b') = c' regardless of whether or not the rotation is proper or improper because
the det(A) will be -1 for improper rotation and fixes the sign for the pseudovector

"""

def test_random_vector_rotation():
    u = np.random.randint(1, high=3)
    v = np.random.randint(1, high=3)
    if u == v:
        v = (v + 1) % 3

    # generate a rotation matrix
    # then make another rotation matrix that
    # has a negative determinant by swapping the rows
    R = special_ortho_group.rvs(3)
    Ri = R.copy()
    Ri[[u, v]] = R[[v, u]]
         
    v1 = np.random.rand(3)
    v2 = np.random.rand(3)
    v3 = np.cross(v1, v2)

    # do a proper rotation, improper rotation, and a improper with the determinant sign accounted for
    # the improper rotation without the determinant sign accounted for should give a negative cross product
    # for the new transformed vector v1 and v2
    d, v1r, v2r, v3r, vcr = rotate_vectors(R, v1, v2, v3, False)
    di, v1ri, v2ri, v3ri, vcri = rotate_vectors(Ri, v1, v2, v3, False)
    _, v1pri, v2pri, v3pri, vcpri = rotate_vectors(Ri, v1, v2, v3, True)

    print("Rotation Matrices")
    print(R)
    print("det={% .4f}" % (d))
    print(Ri)
    print("det={% .4f}" % (di))
    print()
    print("Original Vectors")
    print(v1)
    print(v2)
    print(v3)
    print()
    print("Proper Rotation of Vectors")
    print(v1r)
    print(v2r)
    print(v3r)
    print(vcr)
    print()
    print("Improper Rotation of Vectors")
    print(v1ri)
    print(v2ri)
    print(v3ri)
    print(vcri)
    print()
    print("Improper Rotation of Vectors with Determinant Accounted")
    print(v1pri)
    print(v2pri)
    print(v3pri)
    print(vcpri)

"""

A negative scale value applied to one axis can be thought of a reflection over that one axis.
If we apply a uniform negative scale to all the axis, it can be thought of a sequence of reflections.
Reflections can become rotations depending on how many times we apply it.

A negative uniform scaling can be characterized as the following transformation:

A rotation in even dimensions (2D, 4D, etc)
A reflection in odd dimensions (3D, 5D, etc)

We can see this by taking the determinant of the scale matrix and look at the sign to see if the orientation flipped.
For even dimension, we have an even number of negative number multiplication so it is always positive. (rotation)
For odd dimension, we have an odd number of negative number multiplication so it is always negative. (reflection)

"""

def test_negative_scale():
    s = np.random.randint(-1000, high=-1)
    for i in range(10):
        print("Negative Scale of {}x{}".format(i+1, i+1))
        S = s*np.eye(i+1)
        print(np.linalg.det(S))
        print(S)

test_random_vector_rotation()
test_negative_scale()
