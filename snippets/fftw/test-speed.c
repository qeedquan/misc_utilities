#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <fftw3.h>
#include <inttypes.h>

void
p_elapsed(struct timespec *start)
{
	struct timespec now, elapsed;

	clock_gettime(CLOCK_MONOTONIC, &now);

	if ((now.tv_nsec - start->tv_nsec) < 0) {
		elapsed.tv_sec = now.tv_sec - start->tv_sec - 1;
		elapsed.tv_nsec = now.tv_nsec - start->tv_nsec + 1000000000;
	} else {
		elapsed.tv_sec = now.tv_sec - start->tv_sec;
		elapsed.tv_nsec = now.tv_nsec - start->tv_nsec;
	}

	printf("%jd.%jd\n",
	       (intmax_t)elapsed.tv_sec,
	       (intmax_t)elapsed.tv_nsec / 1000000);
}

void
t_vanilla_real(size_t size, unsigned times, double limit, unsigned flags)
{
	size_t i;
	double *in;
	fftw_complex *out;
	fftw_plan plan;
	struct timespec start;

	in = fftw_alloc_real(size);
	out = fftw_alloc_complex(size);
	if (!in || !out)
		goto out;

	memset(out, 0, size);
	for (i = 0; i < size; i++) {
		in[i] = drand48();
	}

	printf("%s(size=%zu, times=%u, flags=%x)\n", __func__, size, times, flags);
	fftw_set_timelimit(limit);
	clock_gettime(CLOCK_MONOTONIC, &start);
	plan = fftw_plan_dft_r2c_1d(size, in, out, 0);
	p_elapsed(&start);

	for (i = 0; i < times; i++) {
		clock_gettime(CLOCK_MONOTONIC, &start);
		fftw_execute(plan);
		p_elapsed(&start);
	}

out:
	if (in && out)
		fftw_destroy_plan(plan);
	fftw_free(in);
	fftw_free(out);
}

int
main(void)
{
	srand48(time(NULL));
	t_vanilla_real(0x100000, 10000, 1, FFTW_ESTIMATE);
	t_vanilla_real(0x1000, 10000, -1, 0);
	return 0;
}
