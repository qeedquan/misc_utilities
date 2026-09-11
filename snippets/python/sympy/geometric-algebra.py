from sympy import *
from galgebra.ga import Ga

def test_od(od):
    a = od.mv('a', 'vector')
    b = od.mv('b', 'vector')
    c = od.mv('c', 'vector')
    d = od.mv('d', 'vector')

    # dot, cross, geometric product
    pprint("a.b={}".format(a|b))
    pprint("a^b={}".format(a^b))
    pprint("a*b={}".format(a*b))
    pprint("a*b*c={}".format(a*b*c))
    pprint("a*b*c*d={}".format(a*b*c*d))
    print()

o2d = Ga('e1 e2', g=[1, 1])
o3d = Ga('e1 e2 e3', g=[1, 1, 1])
test_od(o2d)
test_od(o3d)
