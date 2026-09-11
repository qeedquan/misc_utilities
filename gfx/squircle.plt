#!/usr/bin/gnuplot

# https://en.wikipedia.org/wiki/Squircle
# http://lowrank.net/gnuplot/implicit/solve-e.html

# implicitly (x-a)^4 + (y-b)^4 = r^4
# to plot in gnuplot, represent the equation as 3d surface
# and then find when solution is equal to 0 (z=0)
a = 0
b = 0
f(x, y) = (x-a)**4 + (y-b)**4
set xrange [-1:1]
set yrange [-1:1]
set zrange [*:5]
set view 0, 0
set isosample 1000, 1000
set size square
set cont base
set cntrparam levels incre 1, 1, 1
unset surface
splot f(x, y)
pause -1

# if squircle is centered at (0, 0) (a = 0, b = 0)
# then can be defined parametrically
set parametric
r = 1
plot abs(cos(t)**0.5) * r*sgn(cos(t)), abs(sin(t))**0.5 * r*sgn(sin(t))

pause -1
