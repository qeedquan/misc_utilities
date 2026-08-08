#!/usr/bin/env python3

"""

Given a probability density function, we can generate samples from that distribution by the following method:

1. Integrate the PDF to get the CDF, find the inverse of the CDF function
2. A uniform distribution U can be used to generate the PDF we want by:

X[i] = inverse_cdf(U[i])

Can use PINV method to numerically find the inverse
https://docs.scipy.org/doc/scipy/tutorial/stats/sampling_pinv.html

There are faster algorithms to generate from a specific distribution, but this algorithm is a general method for any PDF

"""

import numpy as np

import matplotlib.pyplot as plt
from scipy.stats import *
from scipy.stats.sampling import NumericalInversePolynomial

def gen(filename, dist):
    urng = np.random.default_rng()
    rng = NumericalInversePolynomial(dist, random_state=urng)
    rvs = rng.rvs(100000)
    x = np.linspace(rvs.min()-0.1, rvs.max()+0.1, num=100000)
    fx = norm.pdf(x)
    plt.plot(x, fx, "r-", label="pdf")
    plt.hist(rvs, bins=50, density=True, alpha=0.8, label="rvs")
    plt.xlabel("x")
    plt.ylabel("PDF(x)")
    plt.title("Samples drawn using PINV method.")
    plt.legend()
    plt.savefig(filename)
    plt.close()

gen("uniform.png", uniform())
gen("normal.png", norm())
gen("chi.png", chi(1))
gen("chi2.png", chi2(1))
gen("cauchy.png", cauchy(scale=0.001))
gen("loggamma.png", loggamma(1))
gen("rayleigh.png", rayleigh)
gen("triangular.png", triang(1))
gen("trapezoid.png", trapezoid(0.2, 0.8))
