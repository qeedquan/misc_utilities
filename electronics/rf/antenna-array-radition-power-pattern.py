#!/usr/bin/env python

import numpy as np
import matplotlib.pyplot as plt

"""

https://scipython.com/book2/chapter-7-matplotlib/examples/modelling-an-antenna-array/

Consider an array of n isotropic antennas at positions d, evenly spaced by d along x-axis from origin:

d[0] = 0, d[1] = d*x, ... d[n-1] = (n-1)*x

If a single antenna produces a radiation vector F(k) where k=(2pi/lambda)*r, the total radiation vector is

F = Sum[j=0, n-1] w[j]*exp(1j*i*k*d[j])*F[k] = A(k)*F(k)

w[j] is the feed coefficient of the jth antenna representing amplitude and phase of it
A(k) is the array factor

relative power gain is |A(k)^2|

"""
def gain(d, w):
    """Return the power as a function of azimuthal angle, phi."""
    phi = np.linspace(0, 2*np.pi, 1000)
    psi = 2*np.pi * d / lam * np.cos(phi)
    j = np.arange(len(w))
    A = np.sum(w[j] * np.exp(j * 1j * psi[:, None]), axis=1)
    g = np.abs(A)**2
    return phi, g

def get_directive_gain(g, minDdBi=-20):
    """Return the "directive gain" of the antenna array producing gain g."""
    DdBi = 10 * np.log10(g / np.max(g))
    return np.clip(DdBi, minDdBi, None)

# Wavelength, antenna spacing, feed coefficients.
lam = 1
d = lam / 2
w = np.array([-1j, 1j, np.sqrt(2)*1j, np.sqrt(2)*-1j])
# Calculate gain and directive gain; plot on a polar chart.
phi, g = gain(d, w)
DdBi = get_directive_gain(g)

fig = plt.figure()
ax = fig.add_subplot(projection='polar')
ax.plot(phi, DdBi)
ax.set_rticks([-20, -15, -10, -5])
ax.set_rlabel_position(45)
plt.show()
