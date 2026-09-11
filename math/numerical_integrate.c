#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <getopt.h>

typedef double (*function)(double x);

double
trapezoid(function f, double begin, double end, int n)
{
	int i;
	double ret = 0;
	double delta = (end - begin) / n;

	ret += f(begin);
	for (i = 1; i < n; i++) {
		ret += 2 * f(begin + i * delta);
	}
	ret += f(end);
	ret *= (delta / 2);
	return ret;
}

double
midpoint(function f, double begin, double end, int n)
{
	int i = 0;
	double ret = 0;
	double mid;
	double delta = (end - begin) / n;

	for (i = 0; i < n; i++) {
		mid = (begin + begin + delta) / 2;
		ret += f(mid);
		begin += delta;
	}
	ret *= delta;
	return ret;
}

double
simpson(function f, double begin, double end, int n)
{
	int i = 0, j;
	double ret = 0;
	double delta = (end - begin) / n;

	ret += f(begin);

	for (j = 1; j < n; j++) {
		ret += (4 - (i << 1)) * f(begin + j * delta);
		i = (i + 1) & 1;
	}
	ret += f(end);
	ret *= (delta / 3);
	return ret;
}

void
usage(void)
{
	fprintf(stderr, "usage: [option]\n");
	fprintf(stderr, "  -b <num>        define left interval at num\n");
	fprintf(stderr, "  -e <num>        define right interval at num\n");
	fprintf(stderr, "  -i <step_size>  define interval step size\n");
	exit(2);
}

int
main(int argc, char *argv[])
{
	double begin = 0, end = 1, interval = 10000;
	int c;
	while ((c = getopt(argc, argv, "b:e:i:h")) != -1) {
		switch (c) {
		case 'b':
			begin = atof(optarg);
			break;
		case 'e':
			end = atof(optarg);
			break;
		case 'i':
			interval = atof(optarg);
			break;
		case 'h':
		default:
			usage();
			break;
		}
	}
	argc -= optind;
	argv += optind;

	printf("midpoint: %lf trapezoid: %lf simpson: %lf\n",
	       midpoint(exp, begin, end, interval),
	       trapezoid(exp, begin, end, interval),
	       simpson(exp, begin, end, interval));
	return 0;
}
