#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt

xs = np.linspace(0.0, 10.0, 1000)
ys = np.sin(xs)
plt.xlabel("x")
plt.ylabel("sin(x)")
plt.title("Sine Wave Plot")
plt.plot(xs, ys, 'b-', linewidth=2.0)
plt.plot(xs[::50], ys[::50], "ro")
plt.savefig("sin.png")
plt.show()
