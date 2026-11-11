"""

https://en.wikipedia.org/wiki/Root_system
https://en.wikipedia.org/wiki/Cartan_matrix

"""

from sympy import *
from sympy.liealgebras.root_system import *
import sys

if len(sys.argv) < 2:
    print("usage: root_system")
    sys.exit(2)

"""

A root system is a configuration of vectors in euclidean space that satisfy some properties

1. The roots span euclidean space
2. The only scalar multiple of a root alpha inside a root system is alpha and -alpha
3. For every root alpha inside the root system, alpha is closed under reflection through a hyperplane perpendicular to alpha
4. If alpha and beta are roots inside the root system, the projection of beta onto the line through alpha is an integer or half-integer
   multiple of alpha

"""

try:
    c = RootSystem(sys.argv[1])
    print(sys.argv[1])
    pprint(c.cartan_matrix())
except:
    print("Unsupport root system")
