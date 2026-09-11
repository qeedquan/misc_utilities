#define _DEFAULT_SOURCE
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

int
main(void)
{
	char state[256], xstate[256];
	long rng[1024];
	size_t i;

	for (i = 0; i < nelem(state); i++)
		state[i] = i;

	initstate(time(NULL), state, sizeof(state));
	memcpy(xstate, state, sizeof(state));

	for (i = 0; i < nelem(rng); i++)
		rng[i] = random();

	setstate(xstate);
	for (i = 0; i < nelem(rng); i++)
		assert(rng[i] == random());

	return 0;
}
