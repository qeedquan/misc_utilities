import numpy as np
import matplotlib.pyplot as plt
from scipy.integrate import odeint

# x'' - a(1-x^2)x' + x = 0
# any second order equation can be written as 2 coupled first order
# let y = x'
# x' = y
# y' = a(1 -x^2)y - x
# (this is not unique, could make another choice like z=x' + x
def dZdt(Z, t, a=1):
    x, y = Z[0], Z[1]
    dxdt = y
    dydt = a*(1-x**2)*y - x
    return [dxdt, dydt]

def random_ic(s=2.0):
    return s*(2*np.random.rand(2) - 1)

fig, axes = plt.subplots(2, 1)

ts = np.linspace(0, 40, 400)
nlines = 20
for ic in [random_ic() for i in range(nlines)]:
    Zs = odeint(dZdt, ic, ts)
    # plot curve
    axes[0].plot(Zs[:,0], Zs[:,1])
    # plot endpoint
    axes[0].plot([Zs[0,0]], [Zs[0,1]], 's')

# plot limit cycles
# x vs dx/dt
avals = np.arange(0.2, 2.0, 0.2)
minpt = int(len(ts) / 2)
for a in avals:
    Zs = odeint(dZdt, random_ic(), ts, args=(a,))
    axes[1].plot(Zs[minpt:,0], Zs[minpt:,1])

plt.show()
