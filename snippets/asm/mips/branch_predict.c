#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

typedef void (*predfn_t)(bool *actual, bool *predict, size_t len, float *hit, float *miss, void *userdata);

void *
xcalloc(size_t nmemb, size_t size)
{
	void *ptr;

	if (nmemb == 0)
		nmemb = 1;
	if (size == 0)
		size = 1;
	ptr = calloc(nmemb, size);
	if (!ptr)
		abort();
	return ptr;
}

void
printbools(const char *prefix, bool *bools, size_t len)
{
	size_t i;

	printf("%s: ", prefix);
	for (i = 0; i < len; i++)
		printf("%d ", bools[i]);
	printf("\n");
}

void
alwayspred(bool *actual, bool *predict, size_t len, float *hit, float *miss, void *userdata)
{
	size_t i, count;

	count = 0;
	for (i = 0; i < len; i++) {
		predict[i] = (uintptr_t)userdata;
		if (predict[i] == actual[i])
			count++;
	}
	*hit = count * 1.0 / len;
	*miss = 1 - *hit;
}

void
b1cpred(bool *actual, bool *predict, size_t len, float *hit, float *miss, void *userdata)
{
	size_t i, count;
	bool matched;

	if (len == 0) {
		*hit = 1;
		*miss = 0;
		return;
	}

	count = 0;
	predict[0] = (uintptr_t)userdata;
	for (i = 0; i < len; i++) {
		matched = false;
		if (predict[i] == actual[i]) {
			matched = true;
			count++;
		}

		if (i + 1 < len)
			predict[i + 1] = (matched) ? predict[i] : !predict[i];
	}

	*hit = count * 1.0 / len;
	*miss = 1 - *hit;
}

void
b2cpred(bool *actual, bool *predict, size_t len, float *hit, float *miss, void *userdata)
{
	size_t i, count;
	bool matched;
	int state;

	if (len == 0) {
		*hit = 1;
		*miss = 0;
		return;
	}

	state = (uintptr_t)userdata;
	count = 0;
	for (i = 0; i < len; i++) {
		switch (state) {
		case 0:
		case 1:
			predict[i] = false;
			break;
		case 2:
		case 3:
			predict[i] = true;
			break;
		default:
			assert(0);
		}

		matched = predict[i] == actual[i];
		if (matched)
			count++;

		if (matched && state < 3)
			state++;
		else if (!matched && state > 0)
			state--;
	}

	*hit = count * 1.0 / len;
	*miss = 1 - *hit;
}

void
test(predfn_t pred, bool *actual, size_t len, void *userdata)
{
	float hit, miss;
	bool *predict;

	predict = xcalloc(len, sizeof(*predict));
	pred(actual, predict, len, &hit, &miss, userdata);
	printbools("actual", actual, len);
	printbools("predict", predict, len);
	printf("hit: %f miss: %f\n\n", hit, miss);
	free(predict);
}

int
main(void)
{
	bool actual1[] = {false, true, true, true};
	bool actual2[] = {false, false, true, false};
	bool actual3[] = {false, false, true, true, true, false};
	bool actual4[] = {false, true, false, false, false, true, false};

	test(alwayspred, actual1, nelem(actual1), (void *)1);
	test(alwayspred, actual2, nelem(actual2), (void *)1);
	test(alwayspred, actual3, nelem(actual3), (void *)1);
	test(alwayspred, actual4, nelem(actual4), (void *)1);

	test(alwayspred, actual1, nelem(actual1), (void *)0);
	test(alwayspred, actual2, nelem(actual2), (void *)0);
	test(alwayspred, actual3, nelem(actual3), (void *)0);
	test(alwayspred, actual4, nelem(actual4), (void *)0);

	test(b1cpred, actual1, nelem(actual1), (void *)1);
	test(b1cpred, actual2, nelem(actual2), (void *)0);
	test(b1cpred, actual3, nelem(actual3), (void *)0);
	test(b1cpred, actual4, nelem(actual4), (void *)0);

	test(b2cpred, actual1, nelem(actual1), (void *)3);
	test(b2cpred, actual2, nelem(actual2), (void *)0);
	test(b2cpred, actual3, nelem(actual3), (void *)0);
	test(b2cpred, actual4, nelem(actual4), (void *)2);

	return 0;
}
