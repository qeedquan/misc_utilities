# Should match the matlab version
import matplotlib.pyplot as plt
import numpy as np
from scipy import signal

A = np.array([[0.0, 1.0], [0.0, -10.0/100]])
B = np.array([[0.0], [1.0/100.0]])
C = np.array([1.0, 0.0])
D = np.array([0.0])
K = np.array([30.0, 70.0])
X = np.array([10, 0.0])

sys = signal.StateSpace(A-B*K, B, C, D)
t, y = signal.step(sys, X0=X)

plt.plot(t, y, 'b--', label="State Space")
plt.show()
