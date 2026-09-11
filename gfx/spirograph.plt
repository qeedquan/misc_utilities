#!/usr/bin/env gnuplot

# http://www.mathematische-basteleien.de/spirographs.htm

set nokey
set parametric
set size ratio -1
set samples 10000
set multiplot layout 2, 5

# hypocycloids
f(t) = (R-r)*cos(r/R*t) + a*cos((1-r/R)*t)
g(t) = (R-r)*sin(r/R*t) - a*sin((1-r/R)*t)

R=5.0; r=1.0; a=1;
plot [t=0:5*2*pi] f(t), g(t)

R=5.0; r=1.0; a=2;
plot [t=0:5*2*pi] f(t), g(t)

R=5.0; r=1.0; a=3;
plot [t=0:5*2*pi] f(t), g(t)

R=5.0; r=1.0; a=4;
plot [t=0:5*2*pi] f(t), g(t)

R=5.0; r=1.02; a=2;
plot [t=0:80*pi] f(t), g(t)

# epicycloids
f(t) = (R+r)*cos(r/R*t) - a*cos((1+r/R)*t)
g(t) = (R+r)*sin(r/R*t) - a*sin((1+r/R)*t)

R=5.0; r=1.0; a=1;
plot [t=0:5*2*pi] f(t), g(t)

R=5.0; r=1.0; a=2;
plot [t=0:5*2*pi] f(t), g(t)

R=5.0; r=1.0; a=3;
plot [t=0:5*2*pi] f(t), g(t)

R=5.0; r=1.0; a=4;
plot [t=0:5*2*pi] f(t), g(t)

R=5.0; r=1.02; a=2;
plot [t=0:80*pi] f(t), g(t)

pause -1
