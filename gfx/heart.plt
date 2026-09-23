#!/usr/bin/env gnuplot

set parametric
plot 16*sin(t)**3, 13*cos(t) - 5*cos(2*t) - 2*cos(3*t) - cos(4*t)
pause -1
