#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt

def logistic(r=2.0, N=100):
    xs = 0.5*np.ones(N)
    for i in np.arange(N-1):
        xs[i+1] = r*xs[i]*(1.0-xs[i])
    return xs

fig, axes = plt.subplots(2, 2)
axes[0, 0].plot(logistic(2.7), 'bo')
axes[1, 0].plot(logistic(3.1), 'ro')
axes[0, 1].plot(logistic(3.5), 'bs')
axes[1, 1].plot(logistic(3.9), 'rs')
plt.show()
