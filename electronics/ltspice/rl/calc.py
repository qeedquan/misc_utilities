#!/usr/bin/env python

from math import *

# https://www.allaboutcircuits.com/textbook/alternating-current/chpt-3/ac-inductor-circuits/
# a pure inductor circuit (AC current with inductor) will have V(t) 90 phase ahead of I(t) and the power can be negative
# implying we can absorb power from the circuit as well as release it (net dissipation of energy is 0), however it cannot
# be realized in practice because this is ideal behavior which have no wire resistance
def reactance(f, L):
    return 2*pi*f*L

def inductive_reactance(E, Xl):
    return E/Xl

print(reactance(60, 10e-3))
print(reactance(120, 10e-3))
print(reactance(2500, 10e-3))

# at 60 hz with 10 mH, find the current of the circuit
# I = E/Xl
print(inductive_reactance(10, reactance(60, 10e-3)))
