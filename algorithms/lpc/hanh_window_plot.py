#!/usr/bin/env python3

"""

The Hann window, also known as raised cosine window is a common window function used for generating overlapping blocks in LPC

"""

from math import floor
from scipy.signal.windows import hann
import matplotlib.pyplot as plt
import numpy as np

sample_rate = 8000
sym = True
window_size = floor(0.03*sample_rate)

window = hann(window_size, sym) 
print(window_size)
print(window)

t = np.array(range(window_size))

fig = plt.figure(figsize=(18, 5))
plt.plot(t, window, 'b')

plt.show()
