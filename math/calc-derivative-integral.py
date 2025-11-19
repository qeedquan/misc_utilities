from sympy.abc import *
from sympy import *
import sys

def print_sdi(expr, iters):
    pprint(expr)
    
    de = expr
    ie = expr
    for i in range(0, iters):
        print("\nDerivative #{}\n".format(i+1))
        de = diff(de, x)
        pprint(de)
    for i in range(0, iters):
        print("\nIntegral #{}\n".format(i+1))
        ie = integrate(ie, x)
        pprint(ie)

if len(sys.argv) < 3:
    print("usage: <order> <expr>")
    sys.exit()

order = int(sys.argv[1])
expr = "".join(sys.argv[2:])

init_printing()
print_sdi(expr, order) 
