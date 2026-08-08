#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>

void
usage(void)
{
	fprintf(stderr, "usage: name\n");
	exit(2);
}

int
main(int argc, char *argv[])
{
	FILE *fp;
	char path[32];
	unsigned long i;

	if (argc < 2)
		usage();

	snprintf(path, sizeof(path), "%s.log", argv[1]);
	fp = fopen(path, "wb");
	if (!fp)
		err(1, "fopen");

	for (i = 0;; i++) {
		fprintf(fp, "%s %lu\n", argv[1], i);
		fflush(fp);
		sleep(1);
	}

	fclose(fp);

	return 0;
}
