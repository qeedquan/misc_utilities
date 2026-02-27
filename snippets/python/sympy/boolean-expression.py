#!/usr/bin/env python

from sympy.logic import simplify_logic
from sympy import *
from sympy.abc import *

s = ~(~a | ~b)
print(simplify_logic(s))

s = ~(~(a | b & c) | ~(a & ~b))
print(simplify_logic(s))

s = ~((~a & ~b) | ~c) & ~(~a | b)
print(simplify_logic(s))

s = (a | (b & c)) & (a & ~b)
print(simplify_logic(s))

s = (~(~a & (~b | ~c)) & ~(~a | b))
print(simplify_logic(s))
