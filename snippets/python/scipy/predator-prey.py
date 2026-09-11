import numpy as np
import matplotlib.pyplot as plt
from scipy.integrate import odeint

# dx/dt = ax - bxy
# dy/dt = -cy + dxy
def dZdt(Z, t, a=1, b=1, c=1, d=1):
    x, y = Z[0], Z[1]
    dxdt = x*(a - b*y)
    dydt = -y*(c - d*x)
    return [dxdt, dydt]

ts = np.linspace(0, 12, 100)
Z0 = [1.5, 1.0]
Zs = odeint(dZdt, Z0, ts, args=(1,1,1,1))
prey = Zs[:,0]
pred = Zs[:,1]

fig, axes = plt.subplots(2)
# time plot
axes[0].plot(ts, prey, "+", label="Rabbits")
axes[0].plot(ts, pred, "x", label="Foxes")
axes[0].legend()

# phase plot, shows how many rabbits vs foxes for any value
# since the equation is autonomous, no dependence on time
ics = np.arange(1.0, 3.0, 0.1)
for r in ics:
    Z0 = [r, 1.0]
    Zs = odeint(dZdt, Z0, ts)
    axes[1].plot(Zs[:,0], Zs[:,1], "-")

plt.show()
