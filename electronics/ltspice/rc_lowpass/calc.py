#!/usr/bin/env python

# http://sim.okawa-denshi.jp/en/CRtool.php
# A RC circuit can act as a low pass filter when fed different AC frequencies if we hook
# them up in a serial way
# We can calculate various values of the filters using the formulas below

from math import *

def cutoff_frequency(R, C):
    return 1/(2*pi*R*C)

def risetime(R, C, t0, t1):
    return R*C*log(t1/t0)

def print_risetimes():
    R = 1e3
    C = 1e-7
    t = [
        [0.1, 0.9],
    ]
    for i in t:
        print("R = %f C = %.8f t0 %f t1 %f risetime %f" % (R, C, i[0], i[1], risetime(R, C, i[0], i[1])))

def print_cutoff_frequencies():
    R = 1e3
    C = 1e-7
    # there is one pole for an RC filter, which is negative of the cut off frequency
    f = cutoff_frequency(R, C)
    p = -f
    print("R = %f C = %.8f Cutoff Frequency %f Pole %f" % (R, C, f, p))

# p = 1/RC
# p / (s + p)
def transfer_function(s, R, C):
    p = 1 / (R*C)
    print(p)
    return p / (s + p)

print_risetimes()
print_cutoff_frequencies()
print(transfer_function(1+3j, 1e3, 1e-7))
