#!/usr/bin/env python3

"""

https://tutorial.math.lamar.edu/classes/de/IntroSecondOrder.aspx

2nd order constant homogenous differential equation are equations of the form

ay'' + by' + cy = 0

The general solution is of the form
y(t) = c1*exp(r1*t) + c2*exp(r2*t)

To solve for r1 and r2, we can treat the equation as a quadratic equation where

r1 = [-b + sqrt(b^2 - 4ac)] / 2a
r2 = [-b - sqrt(b^2 - 4ac)] / 2a

If there is an initial condition we can solve for the constant c1 and c2

W = [[y1, y2], [y1', y2']]
c1 = [[y0, y2], [y0', y2']] / W
c2 = [[y1, y0], [y1', y0']] / W

W is called the wronskian and determines if there is a solution
y1(t) = exp(r1*t)
y2(t) = exp(r2*t)

Since we have the form of an exponential function, derivative is
y1'(t) = r1*y1(t)
y2'(t) = r2*y2(t)

Plug in t0 for the initial condition (t0 can be nonzero)

"""

import cmath
import numpy as np
import matplotlib.pyplot as plt

def det2(m):
    return m[0][0]*m[1][1] - m[0][1]*m[1][0]

def solve(a, b, c, y0=None, y0p=None, t0=0):
    d = cmath.sqrt(b*b - 4*a*c)
    r1 = (-b + d) / (2*a)
    r2 = (-b - d) / (2*a)
    if y0 == None or y0p == None:
        return [r1, r2]
    
    y1 = cmath.exp(r1*t0)
    y2 = cmath.exp(r2*t0)
    y1p = r1*y1
    y2p = r2*y2
    W = det2([[y1, y2], [y1p, y2p]])
    if W == 0:
        return [r1, r2, None, None]

    c1 = det2([[y0, y2], [y0p, y2p]]) / W
    c2 = det2([[y1, y0], [y1p, y0p]]) / W
    return [r1, r2, c1, c2]

def plot(name, a, b, c, y0, y0p, t0):
    r = solve(a, b, c, y0, y0p, t0)
    assert(len(r) == 4)
    
    f = lambda t: abs(r[2]*cmath.exp(r[0]*t) + r[3]*cmath.exp(r[1]*t))
    ts = np.linspace(0, 10, 100)
    ys = np.array([f(t) for t in ts])
    plt.clf()
    plt.plot(ts, ys, label="a={} b={} c={} y(t0)={} y'(t0)={} t0={}\nr0={}\nr1={}\nc1={}\nc2={}".
            format(a, b, c, y0, y0p, t0, r[0], r[1], r[2], r[3]))
    plt.legend()
    plt.savefig(name)

plot("solution_1.png", 1, 11, 24, 0, -7, 0)
plot("solution_2.png", 1, 3, -10, 4, -2, 0)
