#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <err.h>

void
usage(void)
{
	fprintf(stderr, "usage: file size\n");
	exit(2);
}

int
main(int argc, char *argv[])
{
	FILE *fp;
	char buf[32768];
	size_t i, len, size;

	if (argc != 3)
		usage();

	fp = fopen(argv[1], "wb+");
	if (!fp)
		err(1, "open");

	memset(buf, 0, sizeof(buf));
	size = strtoul(argv[2], NULL, 0);
	for (i = 0; i < size; i += len) {
		len = min(size - i, sizeof(buf));
		fwrite(buf, len, 1, fp);
	}

	fclose(fp);
	return 0;
}
