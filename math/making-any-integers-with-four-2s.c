/*

Given exactly four instances of the digit 2 and some target natural number, use any mathematical operations to generate the target number with these 2s, using no other digits.

https://eli.thegreenplace.net/2025/making-any-integer-with-four-2s/

Dirac solution:

-log[sqrt(2 + 2)](log2(sqrt_n(2))) where sqrt_n is a sqrt iterated n times where n is the natural number
A bit of a cheat but there is 4 2s in the above equation.

*/

#include <stdio.h>
#include <math.h>

long double
solve(unsigned n)
{
	long double x;
	unsigned i;

	x = 2;
	for (i = 0; i < n; i++)
		x = sqrt(x);
	return -log2(log2(x));
}

int
main()
{
	unsigned i;

	for (i = 0; i <= 50; i++)
		printf("%u %Lf\n", i, solve(i));
	return 0;
}
