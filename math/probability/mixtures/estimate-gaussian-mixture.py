#!/usr/bin/env python

from sklearn.mixture import GaussianMixture
from scipy import stats
import numpy as np
import sys
from math import sqrt, log, exp, pi

"""

https://www.kaggle.com/charel/learn-by-example-expectation-maximization

The way to estimate a single gaussian from the dataset is by taking the mean and standard deviation of the data,
and then feed that as parameters of a gaussian distributon in order to generate the probability density function.

For multiple mixture models, use expectation-maximization algorithm to estimate the parameters

"""

class Gaussian:
    def __init__(self, mu, sigma):
        self.mu = mu
        self.sigma = sigma

    def pdf(self, datum):
        u = (datum - self.mu) / abs(self.sigma1)
        y = (1 / (sqrt(2 * pi) * abs(self.sigma))) * exp(-u * u / 2)
        return y

    def __repr__(self):
        return 'Gaussian({0:4.6}, {1:4.6})'.format(self.mu, self.sigma)

def main():
    if len(sys.argv) < 3:
        print("usage: file components")
        exit(0)

    filename = sys.argv[1]
    components = int(sys.argv[2])

    data = np.genfromtxt(filename, comments='#', delimiter='\n')
    best_single = Gaussian(np.mean(data), np.std(data))
    print("Best single estimate")
    print(best_single)

    print()
    print("Mixture estimate")
    gmm = GaussianMixture(n_components=components, tol=0.000001, max_iter=1000)
    gmm.fit(np.expand_dims(data, 1))
    n = 0
    for mu, sd, p in zip(gmm.means_.flatten(), np.sqrt(gmm.covariances_.flatten()), gmm.weights_):
        print('Gaussian {:}: weight = {:.2f} μ = {:.2f} σ = {:.2f}'.format(n, p, mu, sd))
        n += 1

main()
