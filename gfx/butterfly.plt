#!/usr/bin/gnuplot

set dummy t, y
set polar
butterfly(x) = exp(cos(x)) - 2*cos(4*x) + sin(x/12)**5
set samples 800
plot butterfly(t)
pause -1
