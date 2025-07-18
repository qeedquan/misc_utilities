#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt

def lissajous(b=1, a=1, A=2, B=1, d=np.pi/2, col='r-'):
    xs = np.linspace(0, 30, 600)
    plt.plot(A*np.sin(a*xs + d), B*np.sin(b*xs), col, linewidth=1)

lissajous(b=1, col='r-')
lissajous(b=2, col='g-')
lissajous(b=3, col='b-')
lissajous(b=np.pi, col='y-')
lissajous(b=np.e, col='c-')
plt.show()
