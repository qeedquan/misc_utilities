#!/usr/bin/env python

"""

https://en.wikipedia.org/wiki/Arrhenius_equation
https://en.wikipedia.org/wiki/Junction_temperature
https://www.ti.com/lit/an/sprabx4b/sprabx4b.pdf

"""

from math import *
import numpy as np
import matplotlib.pyplot as plt

"""

The Arrhenius equation is used to model electronic component acceleration factor which is a scale factor to the lifetime of the component.
It is applied to temperature operating below the junction temperature (the highest operating temperature of the actual semiconductor in an electronic device.)
Electro-migration is one the dominant wearout mechanisms in semiconductors. The most important variable with respect to electro-migration is the junction temperature (TJ) of the silicon.
Assuming the device is operating within the specified data sheet voltage, the critical variable influencing silicon lifetimeunder electrical bias is the junction temperature (TJ) of the silicon.

An often quoted rule of thumb in electronics reliability for capacitors is that every 10°C increase, the
lifetime approximately halves. For semiconductors, it is a similar change but there is slippage at higher
temperatures.

The equation is a exponential decay model and consists of the following parameters:

AF = Acceleration factor
Ea = Activation energy in eV
k = Boltzmann’s’ constant (8.63 x 10-5 eV/K )
Tuse = Use temperature in K (C + 273)
Tstress = Stress temperature in K ( C+273)

"""
def AF(Ea, Tuse, Tstress):
    Tuse += 273
    Tstress += 273
    k = 8.63 * 1e-5
    return exp(Ea/k * (1.0/Tuse - 1.0/Tstress))

# replicates the graph in TI in exponential scale where the temperature is varied to show the accelerating factor change based on temperature
# the scale factor shows that running under lower temperatures increases the lifetime of the component
x = np.linspace(45, 105)
y = [AF(0.7, x, 105) for x in x]
plt.plot(x, y)
plt.show()
