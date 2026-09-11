import numpy as np
from matplotlib import pyplot as plt

data = np.genfromtxt('data.csv', delimiter=',')
x, y = data.T
plt.scatter(x,y)
plt.show()
plt.close()

