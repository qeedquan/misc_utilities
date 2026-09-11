#!/usr/bin/env python

# http://sim.okawa-denshi.jp/en/CRhikeisan.htm
# http://sim.okawa-denshi.jp/en/CRtool.php
# A RC circuit can act as a high pass filter when fed different AC frequencies
# if we hook them in parallel to each other
# We can calculate various values of the filters using the formulas below

from math import *

def cutoff_frequency(R, C):
    return 1/(2*pi*R*C)

def risetime(R, C, t0, t1):
    return R*C*log(t0/t1)

def print_risetimes():
    R = 25*1e3
    C = 500*1e-9 
    t = [
        [0.1, 1],
    ]
    for i in t:
        print("R = %f C = %.8f t0 %f t1 %f risetime %f" % (R, C, i[0], i[1], risetime(R, C, i[0], i[1])))

def print_cutoff_frequencies():
    R = 25*1e3
    C = 500*1e-9
    # there is one pole for an RC filter, which is negative of the cut off frequency
    f = cutoff_frequency(R, C)
    p = -f
    print("R = %f C = %.8f Cutoff Frequency %f Pole %f" % (R, C, f, p))

# p = 1/RC
# s / (s + p)
def transfer_function(s, R, C):
    p = 1/(R*C)
    print(p)
    return s / (s + p)

print_risetimes()
print_cutoff_frequencies()
print(transfer_function(1+3j, 25*1e3, 500*1e-9))
