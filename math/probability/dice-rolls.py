#!/usr/bin/env python

import numpy as np

"""

https://apoorvaj.io/convolution-and-probability-theory/
https://stats.stackexchange.com/questions/153726/proof-of-algebraic-formula-for-the-sum-of-two-dice-toss-as-a-convolution

Given a dice of N sides, we want to find the probability of getting a value as we toss it M times.
This can be viewed tossing D different dices M times, but each different dice has to have the same number of sides N,
otherwise the convolution trick does not work.

We can find the probability of a value being reached by the following:
1. Initialize the dice with the probability of landing on a side.
2. For M iterations, convolve that array that initialized array.
3. This will generate an array at the end that stores the probability of each value being obtained at the end.

By applying convolution over and over on the same distribution P, the distribution tends torward a gaussian by the central limit theorem.
There are some cases where this never tends torward a normal distribution (cauchy distribution being a counter example).

"""

def dice(sides, iterations):
    prob0 = prob = sides*[1/sides]
    for i in range(1, iterations):
        prob = np.convolve(prob, prob0)
    print("Dice: %d Sides %d Iterations" % (sides, iterations))
    for i in range(len(prob)):
        print("Probability of landing: %d: %.6f%%" % (i+iterations, prob[i]*100))
    print()

dice(6, 1)
dice(6, 2)
dice(6, 3)

dice(3, 2)
dice(3, 50)
