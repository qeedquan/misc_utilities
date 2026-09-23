#!/usr/bin/env gnuplot

# Smooth clipping function based on a parameter k
# k >= 1 is the "clipping hardness". 1 gives a smooth clipping, and a high value gives hardclipping.
# Don't set k too high, because the formula use the pow() function, which use exp() and
# would overflow easily. 100 seems to be a reasonable value for "hardclipping"
# https://www.musicdsp.org/en/latest/Effects/104-variable-hardness-clipping-function.html

sign(x) = (x > 0) ? 1 : ((x == 0) ? 0 : -1)
clip(x, k) = sign(x) * atan(abs(x)**k)**(1/k)
aclip(x, a) = atan(x*a)/atan(a)

set key right bottom

set term png
set output 'clip.png'
plot clip(x, 0.25), clip(x, 0.5), clip(x, 0.75), clip(x, 1), clip(x, 2), clip(x, 5), clip(x, 10)

clear
set output 'aclip.png' 
plot aclip(x, 0.25), aclip(x, 0.5), aclip(x, 0.75), aclip(x, 1), aclip(x, 2), aclip(x, 5), aclip(x, 10)

