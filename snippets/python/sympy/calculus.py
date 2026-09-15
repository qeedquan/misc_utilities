from sympy import *

x, y = symbols('x y')
print(Derivative(x*x, x).doit())
print(Derivative(sin(x), x).doit())
print(Derivative(exp(x), x).doit())
print(Derivative(x*x - exp(x) + sin(x)*cos(x)/exp(2*x*x*x), x).doit())

print(Integral(x*3, x).doit())
print(Integral(exp(x), x).doit())
print(Integral(sin(x), x).doit())
print(Integral(exp(-x**2), x).doit())

print(x*x*x + 3*x)

A = Matrix([[x*y, 0], [0, x+24*x-14*(y*x*x*x*y+510)]])
print(A)
print(A**5)
print(A**-1)
print("transpose:   {}".format(A.T))
print("exponential: {}".format(exp(A)))
print("determinant: {}".format(det(A)))

R = Matrix([[sinh(x), cosh(x)], [-cosh(x), sinh(x)]])
print(R)
print(exp(R*20))
