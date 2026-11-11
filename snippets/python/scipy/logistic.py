import numpy as np
import matplotlib.pyplot as plt
from scipy.integrate import odeint

# dx/dt = x(1-x)
# x(0) = x0
def logistic(x, t):
    return x*(1 - x)

# closed form solution
def logistic_integral(x0, t):
    if x0 == 0:
        return 0*np.exp(-t)

    A = 1/x0 - 1
    return 1/(1 + A*np.exp(-t))

fig, axes = plt.subplots(2, 1)

ts = np.linspace(0, 10, 100)
ics = np.linspace(0.0, 5.0, 100)
for x0 in ics:
    xs = odeint(logistic, x0, ts)
    axes[0].plot(ts, xs)

    xs = logistic_integral(x0, ts)
    axes[1].plot(ts, xs)

plt.show()
