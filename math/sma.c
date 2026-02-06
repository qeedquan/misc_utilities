// https://en.wikipedia.org/wiki/Moving_average#Simple_moving_average
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

typedef struct {
	int mode;
	double avg;
	size_t rp, wp;
	size_t ws;
	size_t len;
	double val[];
} sma_t;

sma_t *
smanew(int mode, size_t len)
{
	sma_t *a;

	a = calloc(1, sizeof(*a) + len * sizeof(*a->val));
	if (!a)
		return NULL;

	a->mode = mode;
	a->ws = 0;
	a->len = len;
	return a;
}

double
smaupdate(sma_t *a, double v)
{
	size_t i;

	// there a few strategies to handle getting the average
	// before the window is filled fully (the transient response)
	// as more and more samples occur, the effects of transient behavior this dies out
	// and all of them should give a value
	if (a->ws < a->len) {
		switch (a->mode) {
		case 0:
			// first sample is assumed it is the average at this point
			// this implies that the whole buffer is filled with this value
			// since Sum/N = v -> all values in buffer are v
			a->ws = a->len;
			a->wp = a->len - 1;
			a->avg = v;
			for (i = 0; i < a->len; i++)
				a->val[i] = v;
			return a->avg;

		case 1:
		case 2:
		case 3:
			// hold off until we filled the buffer
			a->val[a->wp] = v;
			if (++a->ws < a->len)
				a->wp++;

			// depending on the mode, we could return
			// a 0 before the mean, the current value
			// or the current value/window_size as window_size
			// grows to become N
			if (a->mode == 1)
				return 0;
			if (a->mode == 2)
				return v;
			return v / a->ws;

		default:
			assert(0);
		}
	}

	// next sample we start sliding based on the formula
	// new_avg = prev_avg + 1/n*(p[m] - p[m-n])
	a->val[a->wp] = v;
	a->avg = a->avg + (a->val[a->wp] - a->val[a->rp]) / a->len;
	a->rp = (a->rp + 1) % a->len;
	a->wp = (a->wp + 1) % a->len;

	return a->avg;
}

void
test(int mode, size_t len, double *val, size_t nval)
{
	sma_t *a;
	size_t i;

	a = smanew(mode, len);
	if (!a) {
		fprintf(stderr, "smanew: failed to allocate %zu elements\n", len);
		return;
	}
	printf("test mode %d\n", mode);
	for (i = 0; i < nval; i++)
		printf("%zu %f\n", i, smaupdate(a, val[i]));
	free(a);

	printf("\n");
}

int
main(void)
{
	double v1[64];
	for (size_t i = 0; i < nelem(v1); i++)
		v1[i] = i;
	test(0, 6, v1, nelem(v1));
	test(1, 6, v1, nelem(v1));
	test(2, 6, v1, nelem(v1));
	test(3, 6, v1, nelem(v1));

	double v2[] = {3, 3, 3};
	test(0, 2, v2, nelem(v2));

	return 0;
}
