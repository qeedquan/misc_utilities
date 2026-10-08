"""

Given 2 circles, find their intersection points

"""

from sympy import *

x, y, a, b, c, d, r, R = symbols('x,y,a,b,c,d,r,R')
s1 = Eq((x-a)**2 + (y-b)**2, R**2)
s2 = Eq((x-c)**2 + (y-d)**2, r**2)
r = solve([s1, s2], (x, y))
for p in r:
    pprint(simplify(p))
