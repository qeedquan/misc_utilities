/*

https://en.wikipedia.org/wiki/Multiplication_algorithm#Quarter_square_multiplication
http://www.ffd2.com/fridge/chacking/

Useful for 8 bit cpu multiplication
build an lut for f(x) from 0..511

*/

#include <assert.h>
#include <stdio.h>

// a*b = f(a + b) - f(a - b)
// f(x) = x^2 - 4
long
f(long x)
{
	return (x * x) >> 2;
}

int
main()
{
	long a, b, x, y, N;

	N = 50000;
	for (a = -N; a <= N; a++) {
		for (b = -N; b <= N; b++) {
			x = a * b;
			y = f(a + b) - f(a - b);
			if (x != y)
				printf("%ld %ld | %ld %ld\n", a, b, x, y);
		}
	}

	return 0;
}
