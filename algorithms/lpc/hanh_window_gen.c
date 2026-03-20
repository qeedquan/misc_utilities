/*

https://docs.scipy.org/doc/scipy/reference/generated/scipy.signal.windows.hann.html

The window was named for Julius von Hann, an Austrian meteorologist. It is also known as the Cosine Bell. It is sometimes erroneously referred to as the “Hanning” window, from the use of “hann” as a verb in the original paper and confusion with the very similar Hamming window.

Most references to the Hann window come from the signal processing literature, where it is used as one of many windowing functions for smoothing values. It is also known as an apodization (which means “removing the foot”, i.e. smoothing discontinuities at the beginning and end of the sampled signal) or tapering function.

*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// a symmetric window (alternatives are periodic window)
void
hahn(double *x, size_t n)
{
	size_t i;

	for (i = 0; i < n; i++) {
		x[i] = 0.5 - (0.5 * cos((2 * M_PI * i) / (n - 1)));
	}
}

void
dump(double *x, size_t n)
{
	size_t i;

	printf("%zu\n", n);
	for (i = 0; i < n; i++) {
		printf("%.8E ", x[i]);
		if ((i & 3) == 3)
			printf("\n");
	}
	printf("\n");
}

int
main()
{
	double *x;
	int sample_rate;
	int window_size;

	sample_rate = 8000;
	window_size = 0.03 * sample_rate;
	x = calloc(window_size, sizeof(*x));
	hahn(x, window_size);
	dump(x, window_size);
	free(x);
	return 0;
}
