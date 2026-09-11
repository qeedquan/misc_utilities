"""

https://lpsa.swarthmore.edu/Root_Locus/RootLocusWhy.html

"""

import numpy as np
import matplotlib.pyplot as plt
from sympy.abc import *
from sympy.integrals import inverse_laplace_transform
from sympy.solvers import solve
from sympy import *

def eval_transfer_function(G, R, K0, t0, t1):
    G = G.subs(K, K0)
    R = R.subs(K, K0)
    C = inverse_laplace_transform(G*R, s, t)
    x = np.linspace(t0, t1, 100)
    y = np.array([C.subs(t, ts).evalf().subs(0, 1) for ts in x])
    y = np.array([re(ys) for ys in y])
    return (x, y)

"""

Assume transfer function is
G(s) = 1/(s*(s+3))

In a closed-loop feedback system with gain K
we have the following expression
H(s) = C(s)/R(s) = K*G(s) / (1 + K*G(s))

where C(s) is the output and R(s) is the input

Evaluating H(s) gives us
H(s) = K/(s**2 + 3*s + K)

We can control the gain K so we can vary it and look at the output

The input we will feed in this case is a step response (laplace transform is 1/s)

Since it is time consuming to try various K to see what works best, we can find K
another way

Solve for the roots of the poles
For the function

s**2 + 3*s + K gives us the characteristic roots
s = -3/2 +- sqrt(9 - 4K)/2
9 > 4K is overdamped (real roots)
9 < 4K is underdamped (complex roots)
9 = 4K is critically damped
Say we want critical damping, we solve for K to be 4.5

"""
D = s**2 + 3*s + K
G1 = K/D
R1 = 1/s
x1, y1 = eval_transfer_function(G1, R1, 1, 0, 10)
x2, y2 = eval_transfer_function(G1, R1, 10, 0, 10)
x3, y3 = eval_transfer_function(G1, R1, 100, 0, 10)
x4, y4 = eval_transfer_function(G1, R1, 4.5, 0, 10)

print("Roots", solve(D, s))

plt.xlabel('Time')
plt.ylabel('Amplitude')
plt.figure(1)
plt.plot(x1, y1, label='K=1')
plt.plot(x2, y2, label='K=10')
plt.plot(x3, y3, label='K=100')
plt.plot(x4, y4, label='K=4.5')
plt.legend(loc='best')
plt.savefig('simple-gain.png')
plt.show()
