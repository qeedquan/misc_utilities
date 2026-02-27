#!/usr/bin/env python

"""

https://en.wikipedia.org/wiki/Examples_of_Markov_chains
https://people.math.harvard.edu/~knill/teaching/math19b_2011/handouts/lecture33.pdf

"""

import numpy as np
import random
import sys

"""

A transition matrix P is a probability matrix where all entries are [0, 1] and each of the row sums to 1.
It records the probability of going from state i to state j for all pairs (i, j).

The transition matrix P is applied to an initial state vector x to evolve it

x[n+1] = x[n]*P

This can be rewritten as

x[n+1] = x[0]*P^(n+1)

All markov matrices have an eigenvalue of 1, all other eigenvalues absolute value will be less than or equal to 1.
The product of two markov matrices is another markov matrix.

If we want to generate random values following the probability of the chain,
set the initial vector state as a probability of the event occuring initially (the vector components all sum to 1 and is between [0, 1]).

If the transition matrix converges to a stable value (the transition matrix is regular), the initial state vector values is not so important and will also converge.
The vector value components will converge in such a way that this condition is satisfied:

The initial vector components sum = The final vector components sum
We can think the sum of the components as a sort of convex hull for the all the states.

"""

def sim(P0, x0, n):
    C = np.array([0.0] * len(P0))
    P = np.eye(len(P0))
    x = x0
    for i in range(n):
        C[choose(x[0])] += 1
        x = np.dot(x, P0)
        P = np.dot(P, P0)
   
    # the random selection we make should be the same as the steady state vector after a long time running
    # (the initial vector has to be a probability vector though)
    C /= n

    # the eigenvectors/eigenvalues of the initial and final transition matrix P can be different
    (ew0, ev0) = np.linalg.eig(P0)
    (ew, ev) = np.linalg.eig(P)

    print("Final states")
    # these vector should be the same
    print(x, np.dot(x0, P))
    print("Counters")
    print(C)
    print("Eigenvalues")
    print(ew0)
    print(ew)
    print("Eigenvectors")
    print(ev0)
    print(ev)
    print()

"""

Given a probability vector, select a random choice out of them

"""

def choose(x):
    u = random.uniform(0, 1)
    i = 0
    for v in x:
        if u <= v:
            return i
        u -= v
        i += 1
    return i


"""

The weather markov chain has two states, sunny or cloudy.

"""

def test_weather():
    print("Weather Markov Chain")
    P = np.array([[0.9, 0.1], [0.5, 0.5]])
    x1 = np.array([[1, 0]])
    x2 = np.array([[0.2, 0.8]])

    # does not follow the constraint of the initial probability vector
    x3 = np.array([[5, 3.6]])

    sim(P, x1, 10000)
    sim(P, x2, 10000)
    sim(P, x3, 10000)

"""

The stock market markov chain has three states, bullish, bearish, or stagnant

"""

def test_stock_market():
    print("Stock Market Markov Chain")
    P = np.array([[0.9, 0.075, 0.025], [0.15, 0.8, 0.05], [0.25, 0.25, 0.5]])
    x1 = np.array([[0, 1, 0]])
    x2 = np.array([[0, 0.2, 0.8]])

    # does not follow the constraint of the initial probability vector
    x3 = np.array([[1, 2, 3]])

    sim(P, x1, 10000)
    sim(P, x2, 10000)
    sim(P, x3, 2)

def main():
    random.seed()
    test_weather()
    test_stock_market()

main()
