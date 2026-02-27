"""

https://en.wikipedia.org/wiki/Routh%E2%80%93Hurwitz_stability_criterion
http://et.engr.iupui.edu/~skoskie/ECE680/Routh.pdf
http://control.asu.edu/Classes/MAE318/318Lecture10.pdf
http://blog.codelv.com/2013/02/control-systems-in-python-part-2.html

"""

from sympy import *
from sympy.abc import *

"""

A system is considered stable if the poles of its transfer functions all have negative real part
A way to figure this out for large polynomials is to use the routh table

A routh table is the following table
s^n    a0 a2 a4 a6 ...
s^n-1  a1 a3 a5 a7 ...
s^n-2  b1 b2 b3 b4 ...
s^n-3  c1 c2 c3 c4 ...
...
s^2    e1 e2
s^1    f1
s^0    g0

Where the coefficients are
b1 = (a1*a2-a0*a3)/a1
b2 = (a1*a4-a0*a5)/a1
b3 = (a1*a6-a0*a7)/a1
...
c1 = (b1*a3-a1*b2)/b1
c2 = (b1*a5-a1*b3)/b1
c3 = (b1*a7-a1*b4)/b1
...
d1 = (c1*b2-b1*c2)/c1
d2 = (c1*b3-b1*c3)/c1

Then verify the table has all positive coefficients in the first row (all coefficients of a are positive)
and all the coefficient in the first column be positive, if there are any sign changes in the column
they tell us that there is a root that has positive real part (make it unstable)

"""

# does not work with all zero rows
def routh_hurwitz(Ts,*args):
    den = Poly(Ts.as_numer_denom()[1],s).all_coeffs()
    n = len(den)-1
    if n < 2:
        return None
    m = zeros(n + 1)
    
    c = [den[i] for i in range(0, n + 1, 2)]
    for i in range(0,len(c)):
        m[0, i] = c[i]
    
    c = [den[i] for i in range(1, n + 1, 2)]
    for i in range(0,len(c)):
        m[1, i] = c[i]
 
    for i in range(2, n+1):
        for j in range(1, n):
            if m[i-1,j] == 0:
                m[i, j-1] = m[i-2, j]
            elif m[i-1, 0] != 0:
                m[i, j-1] = ((m[i-1, 0]*m[i-2, j]-m[i-2, 0]*m[i-1, j])/m[i-1, 0]).together().simplify()
    
    m = m.col_insert(0, Matrix(([s**(n-i) for i in range(0,n+1)])))
    return m

# stable
pprint(routh_hurwitz(1/(s**4 + 2*s**3 + 3*s**2 + s + 1)))
pprint(routh_hurwitz(1/(s*s*s + a*s*s + b*s + c)))
pprint(routh_hurwitz((k*s**2 + s + 1)/(s**4 + 2*s**3 + (3+k)*s**2 + (1+k)*s + (1+k))))

# unstable
pprint(routh_hurwitz(1/(s**3 + s**2 + s + 2)))
pprint(routh_hurwitz(1000/(s**3 + 10*s**2 + 31*s + 1030)))

# should fail on this one since it has some all zeros row
pprint(routh_hurwitz(1/(s**5 + 2*s**4 + 24*s**3 + 48*s**2 - 50)))
