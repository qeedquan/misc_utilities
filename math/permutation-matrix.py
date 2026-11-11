#!/usr/bin/env python3

"""

https://en.wikipedia.org/wiki/Permutation_matrix

A permutation is a matrix with {0, 1} entries that when multiplied with a vector, it swaps the indices of the elements.
Every permutation matrix P is orthogonal, with its inverse equal to its transpose: P^-1 = P^T (permutations can be viewed as a of rotation of sorts)
Permutation matrices can be characterized as the orthogonal matrices whose entries are all non-negative.

Every permutation matrix is doubly stochastic
A doubly stochastic matrix (also called bistochastic matrix) is a square matrix X where each rows and columns sum to 1:
Sum[i] x[i][j] = Sum[j] x[i][j] = 1

The rows and columns of a permutation matrix can be thought of as the standard basis for the space of 1xK or Kx1 vectors.

The permutation matrix is full-rank.

Raising a permutation matrix to some power gives us the identity matrix (the matrix is cyclic)

"""

import numpy as np

"""

Example:

[1 2 3 4] ->
[3 2 4 1]

To construct a permutation matrix for this transformation:

1. For each row of the matrix corresponds to an index of the elements, it should only have one 1 in the entire row and all other entries in that row is 0.
2. The location of the 1 in the row of the matrix corresponds to the location where we want to map it to, in case of 1->3, we want to put [1][3] = 1 (assuming 1 index)

So the matrix we end up with is:

[0 0 1 0]
[0 1 0 0]
[0 0 0 1]
[1 0 0 0]

To convert back to the top, we just transpose the matrix

"""

P = np.array([[0, 0, 1, 0], [0, 1, 0, 0], [0, 0, 0, 1], [1, 0, 0, 0]])
x = np.array([1, 2, 3, 4])
y = P @ x
print(y)
print(np.transpose(P) @ y)

