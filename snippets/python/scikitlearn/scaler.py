#!/usr/bin/env python3

import numpy as np
from sklearn.preprocessing import *

"""

Usual learning algorithms expect the data to be normalized as follows:

z = (x - mean) / standard_deviation

Where it moves the mean to zero and scales the data to unit variance
For multiple dimensions, each dimension is treated independently where each of them have a different mean and variance in each dimension

"""

def test_standard_scaler(dim):
    scaler = StandardScaler()
    data = np.random.rand(1000, dim)*10000
    mu = np.mean(data, axis=0)
    sigma = np.std(data, axis=0)
    scaler.fit(data)

    X0 = scaler.transform(data)
    X1 = (data - mu) / sigma
    assert(np.allclose(scaler.transform(data), (data - mu) / sigma))

def main():
    for i in range(1, 50):
        test_standard_scaler(i)

main()
