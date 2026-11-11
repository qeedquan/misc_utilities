"""

https://rh8liuqy.github.io/Uniform_Disk.html

Given a uniform random source, we can generate random points inside a circle with radius r 

To generate random points over the disk with radius r

1. Generate U1 ∼ uniform(0, 1) and U2 ∼ uniform(0,1 ), independent.
2. Set X = r*sqrt(U2) * cos(2*pi * U1)
   Set Y = r*sqrt(U2) * sin(2*pi * U1)

The proof for this involves solving for U1 and U2, finding the Jacobian of the matrix and showing that the det(J) = 1/(pi*r^2) 

"""

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

U1 = np.random.uniform(size = 10000)
U2 = np.random.uniform(size = 10000)
r = 1
X = r * np.sqrt(U2) * np.cos(2 * np.pi * U1)
Y = r * np.sqrt(U2) * np.sin(2 * np.pi * U1)

circle = plt.Circle((0, 0), 1, color='g', fill=False)
fig,ax = plt.subplots()
ax.scatter(x = X, y = Y, s = 0.3)
ax.add_artist(circle)
plt.show()
plt.close()

