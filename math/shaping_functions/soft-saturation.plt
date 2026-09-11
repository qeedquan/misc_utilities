#!/usr/bin/env gnuplot

# https://www.musicdsp.org/en/latest/Effects/42-soft-saturation.html
# This only works for positive values of x. a should be in the range 0..1

f1(x, a) = x
f2(x, a) = a + (x-a)/(1+((x-a)/(1-a))**2)
f3(x, a) = (a+1)/2

saturate(x, a) = (x > 1) ? f3(x, a) : ((x < a) ? f1(x, a) : f2(x, a))

set xrange[0:2]
set yrange[0:2]
plot saturate(x, 0.1), saturate(x, 0.2), saturate(x, 0.3), saturate(x, 0.5), saturate(x, 1)
pause -1

