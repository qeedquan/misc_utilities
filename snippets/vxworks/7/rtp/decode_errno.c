#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

void
usage(void)
{
	fprintf(stderr, "usage: errno ...\n");
	exit(2);
}

int
main(int argc, char *argv[])
{
	int i, x, y;

	if (argc < 2)
		usage();

	for (i = 1; i < argc; i++) {
		x = y = 0;
		if (sscanf(argv[i], "%i-%i", &x, &y) != 2) {
			if (sscanf(argv[i], "%i", &x) == 1)
				y = x;
		}

		for (; x <= y; x++)
			printf("%#x: %s\n", x, strerror(x));
	}

	return 0;
}
