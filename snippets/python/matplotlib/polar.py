#!/usr/bin/env python

import numpy as np
import matplotlib.pyplot as plt

theta = np.arange(0, 100*np.pi, 0.001)
r = 2*np.cos(0.75**2*theta)

fig = plt.figure()
ax = fig.add_subplot(projection='polar')
ax.set_rmax(6)
ax.set_rticks([0.5, 1, 1.5, 2, 2.5, 3, 3.5, 4])
ax.set_rlabel_position(45)
ax.plot(theta, r)
plt.show()
