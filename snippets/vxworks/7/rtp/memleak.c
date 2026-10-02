#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

size_t size = 128 * 1024 * 1024;
int delay = 1;
int cleanup = 0;

void
usage(void)
{
	fprintf(stderr, "usage: [options]\n");
	fprintf(stderr, "  -c: free memory after allocation\n");
	fprintf(stderr, "  -d: delay amount of seconds before next allocation (default: %d)\n", delay);
	fprintf(stderr, "  -h: show this message\n");
	fprintf(stderr, "  -m: allocation size (default: %zu)\n", size);
	exit(2);
}

int
main(int argc, char *argv[])
{
	char *ep, *p;
	size_t i, tot;
	int c;

	while ((c = getopt(argc, argv, "cd:hm:")) != -1) {
		switch (c) {
		case 'c':
			cleanup = 1;
			break;
		case 'd':
			delay = atoi(optarg);
			break;
		case 'h':
			usage();
			break;
		case 'm':
			size = strtoul(optarg, &ep, 0);
			break;
		}
	}

	tot = 0;
	for (;;) {
		p = malloc(size);
		if (!p)
			break;

		tot += size;
		for (i = 0; i < size; i++)
			p[i] = rand();

		printf("%zu bytes allocated (%f megabytes) (first value: %#x)\n", tot, tot / (1024.0 * 1024.0), p[0] & 0xff);

		if (cleanup)
			free(p);

		if (delay > 0)
			sleep(delay);
	}
	return 0;
}
