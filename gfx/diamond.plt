#!/usr/bin/env gnuplot

set parametric
plot for [i=1:8] for [j=1:8] (i)*cos(t)**3, (j)*sin(t)**3
pause -1
