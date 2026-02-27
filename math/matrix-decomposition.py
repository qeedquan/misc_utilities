"""

https://en.wikipedia.org/wiki/Eigendecomposition_of_a_matrix
https://en.wikipedia.org/wiki/Singular_value_decomposition
https://www.cs.cornell.edu/courses/cs4620/2014fa/lectures/polarnotes.pdf

"""


import numpy as np

"""

There are a few ways of factoring a matrix into a form of
M = A*B*C

For a square matrix, if the matrix is diagonalizable, then
it can be eigendecomposed into the following form
M = E*S*inv(E)
where E are the eigenvectors of the matrix M and S is the eigenvalues of the matrix
S is a scaling matrix but E can be a non-rotation matrix, the determinant of E is not 1/-1 usually
This implies that the vectors in the eigendecomposition are not necessarily orthogonal
S can be any complex number scale though
This mainly exists for square matrices, for non-square it sometimes doesn't

Another way is to use SVD decomposition
M = U*E*V
where S and D are a rotation matrix (determinant is 1/-1) and V is a scaling matrix
This implies that S and D are orthonormal and represent rotations/flips
U and V and are not necessarily inverse of each other though
E are positive and non-negative
This decomposition always exist for square and non-square matrices, its a pseudo-inverse for non-square matrices

"""

def test_matrix_factorize(n, s, iters):
    for i in range(iters):
        M = np.random.rand(n, n) * s 
        ew, ev = np.linalg.eig(M)
        ER = ev
        ES = np.diag(ew)
        EN = np.dot(ER, np.dot(ES, np.linalg.inv(ER)))

        U, E, V = np.linalg.svd(M)
        E = np.diag(E)
        UEV = np.dot(U, np.dot(E, V))

        print("Matrix {}x{}".format(n, n))
        print(M)
        print()
        print("Eigendecomposition det={}".format(np.linalg.det(ER)))
        print("Eigenvectors det={}".format(np.linalg.det(ER)))
        print(ER)
        print("Eigenvalues det={}".format(np.linalg.det(ES)))
        print(ES)
        print()

        print("SVD decomposition")
        print("U det={}".format(np.linalg.det(U)))
        print(U)
        print("E det={}".format(np.linalg.det(E)))
        print(E)
        print("V det={}".format(np.linalg.det(V)))
        print(V)
        print()

        assert(np.allclose(M, EN))
        assert(np.allclose(M, UEV))

for i in range(1, 100):
    test_matrix_factorize(i, 1e4, 1)
