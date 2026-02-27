"""

https://en.wikipedia.org/wiki/Leslie_matrix
http://bandicoot.maths.adelaide.edu.au/Leslie_matrix/leslie.cgi

"""

import matplotlib.pyplot as plt
import numpy as np
from sympy import *

"""

f is the birth rate, range is >= 0
s is the survival rate, range is [0, 1]
n0 is the initial population of age classes

To get how the population grows into different classes
n[l+1] = L*n[l]

"""
def leslie_matrix(f, s):
    w = len(f)
    L = [[0 for i in range(w)] for j in range(w)]
    for i in range(w):
        L[0][i] = f[i]
    for i in range(w-1):
        L[i+1][i] = s[i]
    return L

def print_leslie_property(L):
    M = Matrix(L)
    print("Leslie Matrix Properties")
    
    pprint(M)
    print("Eigenvalues")
    ev = M.eigenvals()
    for k in ev:
        pprint(k.evalf())
    
    print("Eigenvectors")
    eg = M.eigenvects()
    for k in eg:
        pprint(k[2])

def plot_leslie_iteration(name, L, n0, iters):
    print("Plotting Leslie Matrix Iteration")
    pprint(Matrix(L))
    pprint(n0)
    print()
    print()

    M = np.array(L)
    n = n0
    w = len(n)
    x = []
    y = [[0 for i in range(iters)] for j in range(w)]
    p = [0] * iters
    for i in range(iters):
        n = np.dot(M, n)
        x.append(i)
        p[i] = sum(n)
        for j in range(w):
            y[j][i] = n[j]
        
    x = np.array(x)
    p = np.array(p)

    fig, ax = plt.subplots(1, 2)
    ax[0].plot(x, p, label='Total Population')
    ax[0].legend(loc='best')
    for i in range(w):
        ax[1].plot(x, np.array(y[i]), label='Age Classes={}'.format(i))
        ax[1].legend(loc='best')

    plt.savefig(name)

init_printing()

n0 = [0, 100, 100, 100, 0, 0, 0, 0]
f = [0, 0, 0.8, 0.6, 0, 0, 0, 0]
s = [0.95, 0.9, 0.75, 0, 0, 0, 0, 0]
L = leslie_matrix(f, s)
plot_leslie_iteration('leslie-1.png', L, n0, 100)

n0 = [0, 1000, 200, 0, 0, 0, 0, 0]
f = [0, 0, 0.2, 0.3, 0.5, 0.1, 0, 0]
s = [1, 0.95, 0.95, 0.7, 0.4, 0.2, 0, 0]
L = leslie_matrix(f, s)
plot_leslie_iteration('leslie-2.png', L, n0, 100)

n0 = [0, 100, 100, 100, 0, 0, 0, 0]
f = [0, 0, 0.8, 0.5, 0, 0, 0, 0]
s = [0.95, 0.9, 0.739, 0, 0, 0, 0, 0]
L = leslie_matrix(f, s)
plot_leslie_iteration('leslie-3.png', L, n0, 100)
