"""

https://en.wikipedia.org/wiki/Square_root_of_a_matrix

B is the sqrt of a matrix A if B*B = A

"""

import numpy as np
from scipy.linalg import sqrtm
from scipy.stats import special_ortho_group

def denman_beaver(A, n=50):
    Y = A
    Z = np.eye(len(A))
    for i in range(n):
        Yn = 0.5*(Y + np.linalg.inv(Z))
        Zn = 0.5*(Z + np.linalg.inv(Y))
        Y = Yn
        Z = Zn
    return (Y, Z)

def babylonian(A, n=50):
    X = np.eye(len(A))
    for i in range(n):
        X = 0.5*(X + np.dot(A, np.linalg.inv(X)))
    return X

def gen_random_matrix(n):
    return np.random.rand(n, n)

def gen_rotation_matrix(n):
    return special_ortho_group.rvs(n)*np.random.randint(-100, 101)

def gen_symmetric_matrix(n):
    A = np.random.randint(-10, 11, size=(n, n))
    A = 0.5*(A + A.T)
    return A

def test(title, gen_matrix, size, iters):
    print("Testing {} matrix".format(title))
    for i in range(1, size):
        for j in range(iters):
            try:
                A = gen_matrix(i)
                d = np.linalg.det(A)
                Y, _ = denman_beaver(A)
                X = babylonian(A)
                Z = sqrtm(A)
                print("{}x{} matrix (det {})".format(i, i, d))
                print(A)
                print("Denman Beaver")
                print(np.dot(Y, Y))
                print("Babylonian")
                print(np.dot(X, X))
                print("Scipy")
                print(np.dot(Z, Z))
                print()
            except:
                pass

# iteration methods above tend to fail on random and symmetric matrices
test("random", gen_random_matrix, 5, 10)
test("symmetric", gen_symmetric_matrix, 5, 10)

# for rotation matrices, the iteration methods work
test("rotation", gen_rotation_matrix, 5, 10)

