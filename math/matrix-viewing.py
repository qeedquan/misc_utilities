"""

http://www.songho.ca/opengl/gl_projectionmatrix.html
http://www.songho.ca/opengl/gl_transform.html
https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/gluPerspective.xml
https://lmb.informatik.uni-freiburg.de/people/reisert/opengl/doc/glFrustum.html

"""

from sympy.abc import *
from sympy import *

"""

Orthographic matrix view volume is along the negative z-axis
so f is more negative than n so n>f, so that is why there is
(n+f)/(n-f) rather than (f+n)/(f-n)

"""


def ortho(l, r, b, t, n, f):
    return Matrix([
        [2/(r-l), 0, 0, -(r+l)/(r-l)],
        [0, 2/(t-b), 0, -(t+b)/(t-b)],
        [0, 0, 2/(n-f), -(n+f)/(n-f)],
        [0, 0, 0, 1]
    ])


"""

Viewport matrix transforms ndc coordinates to screen space

x_ndc, y_ndc: [-1:1] to
x_scr, y_scr: [0:w]:[0:h]

"""


def viewport(x, y, nx, ny):
    return Matrix([
        [nx/2, 0, nx/2+x],
        [0, ny/2, ny/2+y],
        [0, 0, 1]
    ])


"""

Perspective matrix transforms
x and y in [-1:1] to [-n/z:n/z]

"""


def perspective(n, f):
    return Matrix([
        [n, 0, 0, 0],
        [0, n, 0, 0],
        [0, 0, n+f, -f*n],
        [0, 0, 1, 0]
    ])


def test_ortho():
    p = Matrix([x, y, z, 1])
    print("Orthographic Matrix")
    pprint(ortho(l, r, b, t, n, f))
    print()

    # (x, y, z) are between -1, 1
    print("Canonical Viewing (Y goes up)")
    pprint(ortho(-1, 1, -1, 1, -1, 1)@p)
    print("Canonical Viewing (Y goes down)")
    pprint(ortho(-1, 1, 1, -1, -1, 1)@p)

    # (x, y, z) are from [0,w]:[0,h]:[0,d]
    print("Width, Height, Depth")
    M = ortho(0, w, 0, h, 0, d) @ p
    pprint(M)
    print()

    # substitute the values here to see that it does indeed
    # transform us from [-1,1] in all directions to [0,w]:[0,h]:[0,d]
    pprint(M.subs('x', 0).subs('y', 0).subs('z', 0))
    pprint(M.subs('x', w).subs('y', h).subs('z', d))
    print()


def test_perspective():
    print("Perspective Matrix")

    P = perspective(n, f)
    pprint(P)
    print()

    p = Matrix([x, y, z, 1])
    print("[x y z 1] normal point")
    pprint(expand(P@p))
    print()

    print("[x y z 1] division by z")
    pprint(simplify(P@p/z))
    print()

    # to map a point to [-1:1] we can use
    # [-z/n:z/n] for [x y]
    print("[-z/n -z/n z 1]")
    p = Matrix([-z/n, -z/n, z, 1])
    pprint(simplify(P@p/z))
    print()

    print("[z/n z/n z 1]")
    p = Matrix([z/n, z/n, z, 1])
    pprint(simplify(P@p/z))
    print()

    # this matrix preserves z=n and z=f
    # when z=n x and y are the same values as before the projection
    # when z=f x and y gets scaled by n/f
    print("[x y n 1] near point")
    p = Matrix([x, y, n, 1])
    pprint(expand(P@p/n))
    print()

    print("[x y f 1] far point")
    p = Matrix([x, y, f, 1])
    pprint(expand(P@p/f))
    print()

    # maps [-1:1] to [-n/z:n/z] for x and y
    p = Matrix([-1, -1, z, 1])
    print("[-1 -1 z 1]")
    pprint(expand(P@p/z))
    print()

    p = Matrix([1, 1, z, 1])
    print("[1 1 z 1]")
    pprint(expand(P@p/z))
    print()

    # orthographic matrix combined with perspective is not commutative
    O = ortho(l, r, b, t, n, f)
    p = Matrix([x, y, z, 1])
    
    print("Orthographic x Perspective")
    pprint(simplify(O@P))
    print()

    print("Perspective x Orthographic")
    pprint(simplify(P@O))
    print()

    # it is not well defined when n=f since it leads to division by zero
    print("Orthographic x Perspective (n=f)")
    X = O@P
    X = X.subs(n, f)
    pprint(simplify(X))
    print()

    # when n=-f, (x, y) are transformed normally (but sign is flipped),
    # however z does not scale anymore and is translated by -f
    print("Orthographic x Perspective (n=-f)")
    X = O@P
    X = X.subs(n, -f)
    pprint(simplify(X))
    print()

    # if n=0 and f=1/-1, then the x and y coordinate collapses to a point (0, 0)
    print("Orthographic x Perspective (n=0) (f=1/-1)")
    X = O@P
    Y = X
    X = X.subs(n, 0)
    X = X.subs(f, 1)
    Y = Y.subs(n, 0)
    Y = Y.subs(f, -1)
    pprint(simplify(X))
    pprint(simplify(Y))
    print()

    # since we cannot convert a perspective matrix into a identity matrix
    # we cannot have make a matrix that satisfies orthographic*perspective=orthographic
    # but if we have n=1/-1 and f=0, then we have something close to identity matrix
    # this gives us a 2d orthographic projection if we enforce z=1 for all coordinates (x, y, 1, 1)
    print("Orthographic x Perspective (n=1/-1) (f=0)")
    X = O@P
    Y = O@P
    X = X.subs(n, 1)
    X = X.subs(f, 0)
    Y = Y.subs(n, -1)
    Y = Y.subs(f, 0)
    pprint(simplify(X))
    pprint(simplify(Y))
    print()

    # the glFrustum matrix uses the O*P form with the coordinate system flipped
    OP = O @ P
    OP = simplify(OP * -1)
    print("glFrustum Matrix")
    pprint(OP)
    print()

    # the gluPerspective matrix uses the O*P form with
    # the simplification that the sides are the same l=r, t=b so we have [-l:l] [-t:t] as a bound
    # the coordinate system is also flipped
    
    # we can calculate the left and top side by parameterizing on the viewing angle fovy
    # where the eye is staring straight into the near plane at z=n, we can use the property
    # opposite/adjacent = tan(fovy/2)
    # left/near = tan(fovy/2)
    # left = near * tan(fovy/2)
    # top = left * aspect (if we assume aspect=1, then left=top)
    # frustum(-left, left, -top, top, near, far)
    # if we pass this formula in the frustum we will find that
    # near/left = 1/tan(fovy/2)
    # near/top = 1/tan(fovy/2)
    print("gluPerspective Matrix")
    OP = O @ P
    OP = simplify(OP.subs(r, -l))
    OP = simplify(OP.subs(t, -b))
    OP = simplify(OP.subs(n/l, 1/tan(theta/2)))
    OP = simplify(OP.subs(n/b, 1/tan(theta/2)))
    OP *= -1
    pprint(OP)
    print()
    
    # the order of when we divide by z does not matter due to the properties
    # of matrix multiplication (associativity)
    print("Orthographic Perspective Matrix division by z")
    print("[x y z 1]")
    X = expand((O @ P @ p) / z)
    Y = expand(O @ ((P @ p) / z))
    pprint(X)
    pprint(Y)
    pprint(simplify(X-Y))
    pprint(simplify(Y-X))
    print()

def test_viewport():
    print("Viewport Matrix")
    # ndc coordinate are [-1,1] that maps to [0,w]:[0,h]
    # depending on if we treat the homogeneous coordinate
    # as a point or a vector, it can map to different locations
    # since it will not take into account translation for a vector

    # if we treat the coordinate as a vector, ie, [x y 0]
    # will get us [-w/2,w/2]:[-h/2,h/2]:0
    print("Using [x y 0] representation of a vector")
    p = Matrix([-1, -1, 0])
    pprint(simplify(viewport(0, 0, w, h)@p))
    p = Matrix([1, 1, 0])
    pprint(simplify(viewport(0, 0, w, h)@p))

    # if we treat the coordinate normally [x y 1]
    # then it becomes the standard [0,w]:[0:h]:1
    print("Using [x y 1] representation of a point")
    p = Matrix([-1, -1, 1])
    pprint(simplify(viewport(0, 0, w, h)@p))
    p = Matrix([1, 1, 1])
    pprint(simplify(viewport(0, 0, w, h)@p))


def main():
    init_printing()
    test_ortho()
    test_perspective()
    test_viewport()


main()
