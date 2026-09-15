"""

https://mathematica.stackexchange.com/questions/138737/dc-gain-of-a-transfer-function
https://en.wikipedia.org/wiki/Final_value_theorem
https://ctms.engin.umich.edu/CTMS/index.php?example=Introduction&section=ControlPID

"""

from sympy.abc import *
from sympy import *

"""

The DC gain is *not* the system gain function (the value k in the zpk representation of the transfer function)
k * (s-z1)*(z-2)...
   ----------------
    (s-p1)*(s-p2)...

The k in that equation is called the system gain equation

The DC gain is the value of the output with a step input response as time goes to infinity, we can think of this
as a open-loop response of a step-function

This gives how close the value is to the output we want, ie, if we had 1/20 as a dc gain, that means
that our steady state error is (1-dc_gain) = 1-0.05 = 0.95 of what we want

We can use the final value theorem to calculate the value in the s-domain rather than the t-domain

lim t->oo f(t) = lim s->0 sF(s)

Since F(s) is the transfer function H(s) multiply with the step response (1/s)
We get s * H/s = sH/s = H so we just need to take the limit of transfer function H(s) as s goes to 0 to find the dc gain

"""

def dc_gain(H):
    return limit(H, s, 0)

def pidtf(Kp, Ki, Kd):
    G = Kp + Ki/s + Kd*s
    return (G*H)/(1 + G*H)

# initial transfer function
H = 1/(s*s + 10*s + 20)

# open-loop response, quite poor as the error is ~95% 
print(dc_gain(H))

# we can use feedback with a PID controller to make it better
# while most of these gives the dc gain we want, other factors changed such as
# overshoot, rise time and dc-gain only captures steady state error
print(dc_gain(pidtf(300, 0, 0)))
print(dc_gain(pidtf(300, 10, 0)))
print(dc_gain(pidtf(30, 70, 0)))
print(dc_gain(pidtf(350, 300, 50)))
print(dc_gain(pidtf(350, 796, 32.2)))

