#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <err.h>

void
usage(void)
{
	fprintf(stderr, "usage: command\n");
	exit(2);
}

int
main(int argc, char *argv[])
{
	FILE *fp;
	char buf[8192], *p;

	if (argc < 2)
		usage();

	fp = popen(argv[1], "r");
	if (!fp)
		err(1, "popen");

	while (fgets(buf, sizeof(buf), fp)) {
		p = strchr(buf, '\n');
		if (p)
			*p = '\0';
		printf("popen: '%s'\n", buf);
	}
	fclose(fp);

	return 0;
}
