#!/usr/bin/env python

from math import *

# http://sim.okawa-denshi.jp/en/CRCRkeisan.htm
def H(s, R1, R2, C1, C2):
    G = 1 / (R1*R2*C1*C2)
    a = 1 / (R1*C1)
    b = 1 / (R2*C1)
    c = 1 / (R2*C2)
    return G / (s*s + s*(a+b+c) + G)

def H_s(R1, R2, C1, C2):
    G = 1 / (R1*R2*C1*C2)
    a = 1 / (R1*C1)
    b = 1 / (R2*C1)
    c = 1 / (R2*C2)
    return "%f / (s*s + %fs + %f)" % (G, a+b+c, G)

# https://www.electronicshub.org/passive-low-pass-rc-filters/#Second_Order_Passive_Low_Pass_Filter
def cutoff(R1, R2, C1, C2):
    return 1 / (2*pi*sqrt(R1*C1*R2*C2))

def c1():
    (R1, R2, C1, C2) = (2.0, 2.0, 2.0, 2.0)
    print(H_s(R1, R2, C1, C2))
    print(cutoff(R1, R2, C1, C2))

c1()
