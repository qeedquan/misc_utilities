#!/usr/bin/env gnuplot

fastatan(x) = (x / (1.0 + 0.28 * (x * x)))

plot atan(x), fastatan(x)
pause -1
