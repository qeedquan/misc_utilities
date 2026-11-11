"""

https://docs.scipy.org/doc/scipy/reference/generated/scipy.interpolate.BSpline.html
http://www2.cs.uregina.ca/~anima/408/Notes/Interpolation/UniformBSpline.htm
https://pomax.github.io/bezierinfo/#bsplines
https://en.wikipedia.org/wiki/De_Boor%27s_algorithm
http://www.joshbarczak.com/blog/?p=730

"""

from scipy.interpolate import BSpline
import numpy as np


def uniform_cubic_bspline(t, p):
    a = (1-t)**3/6.0
    b = (4 - 6*t**2 + 3*t**3)/6.0
    c = (1 + 3*t + 3*t**2 - 3*t**3)/6.0
    d = t**3/6.0
    return a*p[0] + b*p[1] + c*p[2] + d*p[3]


def uniform_quadratic_bspline(t, p):
    a = (1-t)**2/2.0
    b = (1 + 2*t - 2*t**2)/2.0
    c = t**2/2.0
    return a*p[0] + b*p[1] + c*p[2]


def cubic_bezier(t, p):
    a = -t**3 + 3*t**2 - 3*t + 1
    b = 3*t**3 - 6*t**2 + 3*t
    c = -3*t**3 + 3*t**2
    d = t**3
    return a*p[0] + b*p[1] + c*p[2] + d*p[3]


def quadratic_bezier(t, p):
    a = t**2 - 2*t + 1
    b = -2*t**2 + 2*t
    c = t**2
    return a*p[0] + b*p[1] + c*p[2]


def uniform_cubic_bspline_to_bezier(p):
    Bzi = np.array([[1, 0, 0, 0],
                    [1, 1/3.0, 0, 0],
                    [1, 2/3.0, 1/3.0, 0],
                    [1, 1, 1, 1]])
    Bs = np.array([[1, 4, 1, 0],
                   [-3, 0, 3, 0],
                   [3, -6, 3, 0],
                   [-1, 3, -3, 1]])/6.0
    return np.dot(Bzi, np.dot(Bs, np.array(p)))


def uniform_quadratic_bspline_to_bezier(p):
    Bzi = np.array([[1, 0, 0],
                    [1, 0.5, 0],
                    [1, 1, 1]])
    Bs = np.array([[1, 1, 0],
                   [-2, 2, 0],
                   [1, -2, 1]])/2.0
    return np.dot(Bzi, np.dot(Bs, np.array(p)))


"""

A general bspline can be evaluated by using De Boor's algorithm.

The special cases are where the knots are all uniform distance from
each other, it becomes a uniform bspline and can be evaluated like a
bezier curve shown above.

When the knot intervals are not uniform, then it is called a Non-Uniform BSpline.
If we add a weights vector to weight every control point (the weights can be outside the range of [0, 1])
then it becomes a Non-Uniform Rational BSpline (NURB).

The way we handle the weights is to treat it as a homogenous coordinate
so if we have a 2D/3D control point, we transform it into the following:
(x, y) -> (x*w, y*w, w)
(x, y, z) -> (x*w, y*w, z*w, w)

The transformed control point are then used as the new control points in algorithm.
After we are done transforming them, we need to project them back by doing a "perspective divide" by w.
NURBs are the most general of these splines and they can represent conic sections like circles and ellipses.

(tx*w, ty*w, w)       -> (tx/w, ty/w, 1)
(tx*w, ty*w, tz*w, w) -> (tx/w, ty/w, tz/w, 1)

t:       Within the range of [knots, len(coeffs)]
points:  The control points
weights: The weights of the control points
degree:  Degree of the spline

"""


def general_bspline(t, points, weights, knots, degree):
    k = int(t)
    p = degree

    d = [points[j + k - p] * weights[j + k - p] for j in range(0, p + 1)]
    h = [weights[j + k - p] for j in range(0, p + 1)]
    for r in range(1, p + 1):
        for j in range(p, r - 1, -1):
            num = (t - knots[j + k - p])
            den = (knots[j + 1 + k - r] - knots[j + k - p])
            alpha = num / den
            d[j] = (1.0 - alpha) * d[j - 1] + alpha * d[j]
            h[j] = (1.0 - alpha) * h[j - 1] + alpha * h[j]

    return d[p]/h[p]


"""

A uniform bspline have the knots of uniform intervals with size
knot vector size = number of control points + spline degree + 1

For a uniform cubic bspline with 4 control points, the knot vector is
[0, 1, 2, 3, 4, 6, 7] (3+4+1)

For a quadratic bspline
[0, 1, 2, 3, 4, 5] (2+3+1)

When this is the case the evaluation reduces something like the bezier
curve for the knots with different coefficients B(t) but the same form
P(t) = B(0)*P(0) + B(1)*P(1) + B(2)*P(2) + B(3)*P(3) (Cubic BSpline)
P(t) = B(0)*P(0) + B(1)*P(1) + B(2)*P(2)             (Quadratic BSpline)
t ranges from [0, 1]

If we have more control points than the number of bspline basis polynomials,
we just evaluate the new control points by shifting them left to right,

t:[0, 1]
P(t) = B(0)*P(0) + B(1)*P(1) + B(2)*P(2) + B(3)*P(3)

t:[0, 1]
P(t) = B(0)*P(1) + B(1)*P(2) + B(2)*P(3) + B(3)*P(4)

t:[0, 1]
P(t) = B(0)*P(2) + B(1)*P(3) + B(2)*P(4) + B(3)*P(5)

and so on.

t always ranges from [0, 1] but we just keep feeding the new control points

In the paradigm of the traditional general bspline however, the domain is not
[0, 1], but it is [knot, len(coeffs)], so it starts from knot and goes up to
the length of the coefficients. This has the advantage of being connected
to all the coefficients instead of splitting it up like above, but it is not normalized
anymore. The bspline function from scipy does this so we have to handle the remapping for it.

We test if the bspline function from scipy when fed with the uniform knots and coefficients
give us the same values as our uniform bspline functions.

"""


def test_uniform_bspline(degree, coeffs, ubspl, dt):
    knots = [i for i in range(0, len(coeffs)+degree+1)]
    weights = [1.0 for i in range(0, len(coeffs))]
    spl = BSpline(knots, coeffs, degree, False)

    eps = 1e-4
    u = degree
    du = dt
    for i in range(len(coeffs)-degree):
        t = 0
        while t <= 1:
            x = spl(u)
            y = ubspl(t, coeffs[i:])
            z = general_bspline(u, coeffs, weights, knots, degree)
            if abs(x-y) >= eps or abs(x-z) >= eps:
                print("mismatched uniform bspline({}): t={} {} {} {}".format(
                    degree, t, x, y, z))
            u += du
            t += dt


"""

Given a set of control points transformed by a spline, we would like to use another spline
with a different set of control points that give the same value as the original spline at
the same parameter t

An example would be a uniform bspline to a bezier curve.
The algorithm to do this is a change of basis, as one would do in a coordinate transform.
This procedure works for any splines of the same order, we can convert
uniform bspline <-> bezier curve <-> hermite <-> camull-rom etc

An example showing the conversion process for a cubic spline
T: [1 t t^2 t^3]
P1: Spline 1 control points
P2: Spline 2 control points
B1: Spline 1 matrix
B2: Spline 2 matrix

T*B1*P1 = T*B2*P2
P1 = B1^-1*B2*P2

This tests if the we can convert spline 2 control points to spline 1 control points.

"""


def test_spline_conversion(spl1, spl2, conv2to1, coeffs, degree, dt):
    eps = 1e-4
    for i in range(len(coeffs)-degree):
        t = 0.0
        while t <= 1:
            p = coeffs[i:i+degree+1]
            q = conv2to1(p)
            x = spl2(t, p)
            y = spl1(t, q)
            if abs(x-y) >= eps:
                print("mismatch conversion degree{}: {} {}", degree, t, x, y)
            t += dt


coeffs = np.random.rand(100)*10000-5000
test_spline_conversion(
    cubic_bezier,
    uniform_cubic_bspline,
    uniform_cubic_bspline_to_bezier,
    coeffs,
    3,
    1e-3
)
test_spline_conversion(
    quadratic_bezier,
    uniform_quadratic_bspline,
    uniform_quadratic_bspline_to_bezier,
    coeffs,
    2,
    1e-3
)

test_uniform_bspline(2, coeffs, uniform_quadratic_bspline, 1e-3)
test_uniform_bspline(3, coeffs, uniform_cubic_bspline, 1e-3)
