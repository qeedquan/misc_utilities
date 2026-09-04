#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt

N = 500
xs = np.random.rand(N)
ys = -np.log(np.random.rand(N))
plt.axis([0, 1, 0, max(ys)])
plt.scatter(xs, ys)
plt.show()
