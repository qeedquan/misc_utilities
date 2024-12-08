"""

https://ctms.engin.umich.edu/CTMS/index.php?example=Introduction&section=ControlPID

"""

import sys
from sympy.abc import *
from sympy import *
from sympy.integrals import inverse_laplace_transform

"""

The output of a PID controller is defined as
u(t) = Kp*e(t) + Ki*Integral e(t) dt + Kp*de/dt

When we take the laplace transform of this equation we find that

Kp + Ki/s + Kd*s

From the laplace table we see that
f'(t) is defined to be sF(s) - f(0-)
integral f(t) dt is defined to be 1/sF(s)

So here we are setting f(0-) to be 0 and F(s) to be 1

Different values of Kp, Ki, Kd gives different transfer functions

The general rule of thumb for these constants is described in the table:

      RISE TIME       OVERSHOOT    SETTLING TIME    SS-ERROR
Kp     Decrease       Increase     Small Change     Decrease
Ki     Decrease       Increase     Increase         Decrease
Kd    Small Change    Decrease     Decrease         No Change

"""

def print_pid_transfer_function(Kp, Ki, Kd):
    G = Kp + Ki/s + Kd*s
    # this is the partial fraction form
    print(G)
    # to put it into zpk (zero-pole-gain) form we just multiply by s/s
    print(expand(s*G)/s)

if len(sys.argv) < 4:
    print("usage: Kp Ki Kd")
    sys.exit()

Kp = float(sys.argv[1])
Ki = float(sys.argv[2])
Kd = float(sys.argv[3])
print_pid_transfer_function(Kp, Ki, Kd)
