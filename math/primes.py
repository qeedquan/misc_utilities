#!/usr/bin/env python3

from sympy import *

"""

In 1991, Harvey Dubner discovered a prime number with a total of 6,400 digits that is composed of all 9's except one 8.
(10^x)-(10^y)-1 always results in all 9’s with a single 8 in the x-y position from the left

"""
p = 10**6400 - 10**6352 - 1
assert(isprime(p) == True)
