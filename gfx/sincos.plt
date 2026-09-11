#!/usr/bin/env gnuplot

# https://en.wikipedia.org/wiki/Parametric_equation#/media/File:Param_03.jpg

set nokey
set multiplot layout 4, 4
set parametric
f(a, b, c, d, j, k, t) = cos(a*t) - cos(b*t)**j
g(a, b, c, d, j, k, t) = sin(c*t) - sin(d*t)**k

a=1; b=2; c=1; d=2;
j=3; k=3;
plot f(a, b, c, d, j, k, t), g(a, b, c, d, j, k, t)

a=2; b=1; c=2; d=1;
j=3; k=3;
plot f(a, b, c, d, j, k, t), g(a, b, c, d, j, k, t)


a=1; b=3; c=1; d=3;
j=3; k=3;
plot f(a, b, c, d, j, k, t), g(a, b, c, d, j, k, t)

a=3; b=1; c=3; d=1;
j=3; k=3;
plot f(a, b, c, d, j, k, t), g(a, b, c, d, j, k, t)

a=1; b=4; c=1; d=4;
j=3; k=3;
plot f(a, b, c, d, j, k, t), g(a, b, c, d, j, k, t)

a=4; b=1; c=4; d=1;
j=3; k=3;
plot f(a, b, c, d, j, k, t), g(a, b, c, d, j, k, t)

a=1; b=5; c=1; d=5;
j=3; k=3;
plot f(a, b, c, d, j, k, t), g(a, b, c, d, j, k, t)

a=5; b=1; c=5; d=1;
j=3; k=3;
plot f(a, b, c, d, j, k, t), g(a, b, c, d, j, k, t)

a=1; b=6; c=1; d=6;
j=3; k=3;
plot f(a, b, c, d, j, k, t), g(a, b, c, d, j, k, t)

a=6; b=1; c=6; d=1;
j=3; k=3;
plot f(a, b, c, d, j, k, t), g(a, b, c, d, j, k, t)

a=1; b=7; c=1; d=7;
j=3; k=3;
plot f(a, b, c, d, j, k, t), g(a, b, c, d, j, k, t)

a=7; b=1; c=7; d=1;
j=3; k=3;
plot f(a, b, c, d, j, k, t), g(a, b, c, d, j, k, t)

a=1; b=2; c=1; d=3;
j=3; k=3;
plot f(a, b, c, d, j, k, t), g(a, b, c, d, j, k, t)

a=1; b=3; c=1; d=2;
j=3; k=3;
plot f(a, b, c, d, j, k, t), g(a, b, c, d, j, k, t)

a=1; b=2; c=2; d=1;
j=3; k=3;
plot f(a, b, c, d, j, k, t), g(a, b, c, d, j, k, t)

a=1; b=3; c=3; d=1;
j=3; k=3;
plot f(a, b, c, d, j, k, t), g(a, b, c, d, j, k, t)

pause -1
