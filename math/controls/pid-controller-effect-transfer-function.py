"""

https://ctms.engin.umich.edu/CTMS/index.php?example=Introduction&section=ControlPID

"""

from sympy.abc import *
from sympy import *

"""

The standard equation for the PID controller transfer function is
G(s) = Kp + Ki/s + Kd*s

This stems from taking the laplace transform of the PID output equation
u(t) = Kp*e(t) + Ki * Integral(e(t)) dt  + Kd*de/dt
where we treat F(s) as 1 and F(0) as 0 to give the above equation

The standard feedback form for a transfer function H(s) with some controller G(s) is
G(s)*H(s)/(1 + G(s)*H(s))

So given a PID controller G(s) and a transfer function H(s)
We can use the above equation to analyze how the PID controller G(s) affect the transfer function H(s)

"""

Kp = Symbol('Kp')
Ki = Symbol('Ki')
Kd = Symbol('Kd')

def pidtf(G, H):
    return simplify(G*H/(1 + G*H))

# For a P controller
G = Kp
H = 1/(s*s + 10*s + 20)
print(pidtf(G, H))

# For a PD controller
G = Kp + Kd*s
print(pidtf(G, H))

# For a PI controller
G = Kp + Ki/s
print(pidtf(G, H))

# For a PID controller
G = Kp + Ki/s + Kd*s
print(pidtf(G, H))
