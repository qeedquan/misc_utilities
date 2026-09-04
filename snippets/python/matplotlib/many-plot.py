#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt

xs = np.linspace(0.0, 10.0, 1000)
ys = np.sin(xs)
plt.title("Many Plot")
plt.axis([0, 2*np.pi, -2, 2])
plt.plot(xs, np.sin(xs), 'r-', label="sin(x)")
plt.plot(xs, np.cos(xs), 'b-', label="cos(x)")
plt.plot(xs, np.tan(xs), 'g-', label="tan(x)")
plt.plot(xs, np.sqrt(xs), 'y-', label="sqrt(x)")
plt.plot(xs, np.sinc(xs), 'c-', label="sinc(x)")
plt.legend()
plt.savefig("manyplot.png")
plt.show()
