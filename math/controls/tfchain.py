#!/usr/bin/env python3

"""

https://www.mathworks.com/help/control/ref/parallel.html
https://www.mathworks.com/help/control/ref/feedback.html
https://www.mathworks.com/help/control/ref/series.html

Transfer functions applys to LTI systems and is defined as
H(s) = Y(s)/X(s) (output/input)
in the laplace domain

In the laplace domain differential equations become rational polynomial equations
so the rules of addition/multiplication is basically adding/multiplying polynomials

Transfer functions can be chained together in the following ways
Cascaded:
    H1*H2*H3*H4...

Parallel:
    H1+H2+H3+H4...

Feedback:
    initial condition
    H_f = H1
    
    Hn represents the next transfer function
    H_f = 1/(1 + H_f*Hn) (Negative Feedback)
    H_f = 1/(1 - H_f*Hn) (Positive Feedback)

"""

from sympy import *

def tfchain(H):
    if len(H) == 0:
        return
    
    GC = H[0]
    GP = H[0]
    GF = H[0]
    GH = H[0]
    for i in range(len(H)-1):
        GC *= H[i+1]
        GP += H[i+1]
        GF = GF/(1 + GF*H[i+1])
        GH = GH/(1 - GH*H[i+1])

    GC = simplify(expand(simplify(GC)))
    GP = simplify(expand(simplify(GP)))
    GF = simplify(expand(simplify(GF)))
    GH = simplify(expand(simplify(GH)))

    print('Transfer Functions = {}'.format(H))
    print('Cascaded           = {}'.format(GC))
    print('Parallel           = {}'.format(GP))
    print('Negative Feedback  = {}'.format(GF))
    print('Positive Feedback  = {}'.format(GH))
    print()

init_printing(use_unicode=True)
s = symbols('s')
tfchain([10/(s*s + 2*s + 10), 5/(s + 5)])
tfchain([(2*s*s + 5*s + 1)/(s*s + 2*s + 3), 5*(s+2)/(s+10)])
