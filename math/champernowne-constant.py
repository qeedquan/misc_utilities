#!/usr/bin/env python

# https://en.wikipedia.org/wiki/Champernowne_constant
# sequences below are related to these constants

# counts how many numbers between two 10**n and 10**(n-1)
def A0(n):
	return (10**n - 10**(n-1))*n

# http://oeis.org/A033714
# This sequence also gives the total count of digits of n below 10^n.
def A033714(n):
	r = 1
	for i in range(1, n):
		r += A0(i)
	return r

for i in range(0, 32):
	print(i, A0(i), A033714(i))
