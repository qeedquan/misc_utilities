#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt

S = 100
N = 1000
data = [sum(np.random.rand(S)) for i in range(N)]
plt.hist(np.array(data), bins=20, range=(40,60))
plt.show()
