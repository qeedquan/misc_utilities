#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt

N = 1000
d = 1.5
xs = np.linspace(-d, d, N)
ys = np.linspace(-d, d, N)*1j
zs = xs.reshape((1, N)) + ys.reshape((N, 1))

r = 2.0
c = -0.123 + 0.745j
for i in range(10):
    zs = zs**2 + c

out = (abs(zs) < r)*1
plt.imshow(out, cmap='Blues', origin='lower')
plt.show()
