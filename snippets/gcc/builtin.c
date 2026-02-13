#include <assert.h>
#include <stdio.h>
#include <stdbit.h>
#include <stdint.h>
#include <limits.h>

int
main()
{
	uint64_t i;

	for (i = 0; i <= 1 << 20; i++) {
		assert(__builtin_popcount(i) == stdc_count_ones(i));
		assert(__builtin_ffs(i) == stdc_first_trailing_one(i));
	}

	i = UINT64_MAX;
	assert(__builtin_ffs(i) == stdc_first_trailing_one(i));

	return 0;
}
