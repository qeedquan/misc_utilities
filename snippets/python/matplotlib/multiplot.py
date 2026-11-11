#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt

xs = np.linspace(0.0, 10.0, 10000)
fig, axes = plt.subplots(3, 3)
ax = axes.flatten()
for i in range(len(ax)):
    ax[i].plot(xs, np.cos(xs*i) + np.sin(xs + 2*np.pi*i/len(axes)))
plt.show()
