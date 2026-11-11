""" 

https://apmonitor.com/pdc/index.php/Main/ModelSimulation
https://apmonitor.com/pdc/index.php/Main/SecondOrderSystems

"""

import numpy as np
import matplotlib.pyplot as plt
from scipy.integrate import odeint
from scipy import signal
from sympy.abc import s, t
from sympy.integrals import inverse_laplace_transform
from math import *

# tau*dy2/dt2 + 2*zeta*tau*dy/dt + y = Kp*u
Kp = 2.0    # gain
tau = 1.0   # time constant
zeta = 0.5 # damping factor
theta = 0.0 # no time delay
du = 1.0    # change in u

# (1) Transfer Function
num = [Kp]
den = [tau**2,2*zeta*tau,1]
sys1 = signal.TransferFunction(num,den)
t1,y1 = signal.step(sys1)

# (2) State Space
A = [[0.0,1.0],[-1.0/tau**2,-2.0*zeta/tau]]
B = [[0.0],[Kp/tau**2]]
C = [1.0,0.0]
D = 0.0
print("ABCD: ", A, B, C, D)
sys2 = signal.StateSpace(A,B,C,D)
t2,y2 = signal.step(sys2)

# (3) ODE Integrator
def model3(x,t):
    y = x[0]
    dydt = x[1]
    dy2dt2 = (-2.0*zeta*tau*dydt - y + Kp*du)/tau**2
    return [dydt,dy2dt2]
t3 = np.linspace(0,25,100)
x3 = odeint(model3,[0,0],t3)
y3 = x3[:,0]

# (4) Closed Form
# ay'' + by' + cy = K*u(t-theta)
# u(t) is the unit step function
# a = tau*tau
# b = 2*zeta*tau
# c = 1
# theta = 0
# This differential equation has 3 different solutions
# based on the value of zeta
def closed(t):
    # magnitude of step function
    M = 1
    if zeta < 1: # underdamped
        z = sqrt(1 - zeta*zeta)
        A = cos(t/tau*z)
        B = zeta/z * sin(t/tau*z)
        v = 1 - exp(-zeta*t/tau)*(A + B)
    elif zeta > 1: # overdamped
        z = sqrt(zeta*zeta - 1)
        A = cosh(t/tau*z)
        B = zeta/z * sinh(t/tau*z)
        v = 1 - exp(-zeta*t/tau)*(A + B)
    else: # critically damped
        v = 1 - (1 + t/tau)*exp(-t/tau)
    return Kp*M*v
y4 = np.array([closed(t) for t in t3])

# (5) Symbolic
# Multiply transfer function G(s) with unit step U(s)
# then take the inverse laplace transform for equation
G = Kp/(tau*tau*s*s + 2*zeta*tau*s + 1)
U = 1/s
Y = inverse_laplace_transform(G*U, s, t)
y5 = np.array([Y.subs(t, ts) for ts in t3])

print("Transfer Function: {}".format(G))

plt.figure(1)
plt.plot(t1,y1*du,'b--',linewidth=5,label='Transfer Function')
plt.plot(t2,y2*du,'g:',linewidth=4,label='State Space')
plt.plot(t3,y3,'r-',linewidth=3,label='ODE Integrator')
plt.plot(t3,y4,'p-',linewidth=2,label='Closed Form')
plt.plot(t3,y5,'c-',linewidth=1,label='Symbolic')
y_ss = Kp * du
plt.plot([0,max(t1)],[y_ss,y_ss],'k:')
plt.xlim([0,max(t1)])
plt.xlabel('Time')
plt.ylabel('Response (y)')
plt.legend(loc='best')
plt.savefig('2nd_order.png')
plt.show()
