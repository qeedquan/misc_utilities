# should match the matlab script
# but we do it symbolicly
import sys
import sympy as sym
import numpy as np
from sympy.abc import s,t,x,y,z
from sympy.integrals import inverse_laplace_transform
import matplotlib.pyplot as plt

# Transfer function
G = 1/(s+1)
# Laplace transform of step, sin, ramp 
U1 = sym.exp(-s)/s
U2 = 20/(s*s + 20**2)
U3 = 1/(s*s)

# Calculate responses
Y1 = G * U1
Y2 = G * U2
Y3 = G * U3

# Inverse Laplace Transform
y1 = inverse_laplace_transform(Y1,s,t)
y2 = inverse_laplace_transform(Y2,s,t)
y3 = inverse_laplace_transform(Y3,s,t)
print(y1)
print(y2)
print(y3)

# generate data for plot
tm = np.linspace(0,10,1000)
us = np.zeros(len(tm))
ys = np.zeros(len(tm))
yt = np.zeros(len(tm))
yu = np.zeros(len(tm))
for i in range(len(ys)):
    ys[i] = y1.subs(t, tm[i])
    yt[i] = y2.subs(t, tm[i])
    yu[i] = y3.subs(t, tm[i])

# plot results
fig, axes = plt.subplots(1, 3)
axes = axes.flatten()
axes[0].set_ylim([0, 1])
axes[1].set_ylim([-0.1, 0.1])
axes[2].set_ylim([0, 10])
axes[0].plot(tm,ys)
axes[1].plot(tm,yt)
axes[2].plot(tm,yu)
plt.xlabel('Time')
plt.show()
