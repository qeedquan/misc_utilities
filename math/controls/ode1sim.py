""" 

https://apmonitor.com/pdc/index.php/Main/ModelSimulation
http://web.mit.edu/2.14/www/Handouts/StateSpaceResponse.pdf

Integrate the differential equation in various ways,
but give the approximately the same solution numerically

"""

from math import *
import numpy as np
import matplotlib.pyplot as plt
from scipy import signal
from scipy.integrate import odeint
from scipy.linalg import expm
from sympy.abc import s, t
from sympy.integrals import inverse_laplace_transform

# Simulate taup * dy/dt = -y + K*u
Kp = 10
taup = 3

# (1) Transfer Function
num = [Kp]
den = [taup,1]
sys1 = signal.TransferFunction(num,den)
t1,y1 = signal.step(sys1)

# (2) State Space
A = -1.0/taup
B = Kp/taup
C = 1.0
D = 0.0
sys2 = signal.StateSpace(A,B,C,D)
t2,y2 = signal.step(sys2)

# (3) ODE Integrator
def model3(y, t):
    u = 1.0
    return (-y + Kp * u)/taup
t3 = np.linspace(0,14,100)
y3 = odeint(model3,0,t3)

# (4) Closed Form
# y' = -ay + b
# a = -1/taup
# b = Kp*u/taup
# k1 is the arbitrary constant based on initial condition
# y(0) = 0
# y(t) = b/a + k1*exp(-a*t)
def closed(t):
    u = 1.0
    a = 1.0/taup
    b = Kp*u/taup
    k1 = -b/a
    return b/a + k1*exp(-a*t)
y4 = np.array([closed(t) for t in t3])

# (5) Symbolic
# We multiply the transfer function G(s) with the unit step response U(s)
# and then take the inverse laplace transform to find the equation
# for evaluation
G = Kp/(taup*s + 1)
U = 1/s
Y = inverse_laplace_transform(G*U, s, t)
y5 = np.array([Y.subs(t, ts) for ts in t3])

# (6) Manual State Space
# This should be the same as (2) but we do it manually rather than using a library
# x(0) = 0 with unit step function gives rise to the following update equation
# x(t) = A**-1*(expm(A*t) - I)*B
# y(t) = C*x(t) + D*step(t)
def stspsim(t):
    MA = np.array([[A]])
    MB = np.array([[B]])
    MC = np.array([[C]])
    MD = np.array([[D]])
    I = np.array([[1.0]])
    
    x = MA**-1 @ (expm(MA*t) - I) @ MB
    y = MC*x + MD*np.heaviside(t, 1)
    return y
y6 = np.array([stspsim(t) for t in t3])
y6 = y6.flatten()

print("Transfer Function: {}".format(Y))

plt.figure(1)
plt.plot(t1,y1,'b--',linewidth=6,label='Transfer Function')
plt.plot(t2,y2,'g:',linewidth=5,label='State Space')
plt.plot(t3,y3,'r-',linewidth=4,label='ODE Integrator')
plt.plot(t3,y4,'p-',linewidth=3,label='Closed Form')
plt.plot(t3,y5,'c-',linewidth=2,label='Symbolic')
plt.plot(t3,y6,'m*',linewidth=1,label='State Space Manual')
plt.xlabel('Time')
plt.ylabel('Response (y)')
plt.legend(loc='best')
plt.savefig('1st_order.png')
plt.show()
