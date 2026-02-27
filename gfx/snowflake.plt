#!/usr/bin/env gnuplot

# https://math.stackexchange.com/questions/808650/an-equation-that-generates-a-beautiful-or-unique-shape-for-motivating-students-i

unset key
set polar
set multiplot layout 4, 4

do for [n=0:16] {
	plot [t=0:2*pi] abs(cos(n*t))**sin(2*n*t)
}

pause -1
