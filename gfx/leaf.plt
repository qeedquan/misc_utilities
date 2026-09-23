#!/usr/bin/env gnuplot

# http://nashath.net/2010/01/single-equation-for-the-graph-of-a-marijuana-leaf/

set polar

f(t) = (1 + 0.9*cos(8*t)) * (1 + 0.1*cos(24*t)) * (0.9 + 0.05*cos(200*t)) * (1 + sin(t))
plot [t=-pi:pi] f(t)
pause -1
