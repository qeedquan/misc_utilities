#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <ftw.h>

int
list(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
	printf("%#08x %8zu %-80s\n", typeflag, sb->st_size, fpath);
	(void)ftwbuf;
	return 0;
}

void
usage(void)
{
	fprintf(stderr, "usage: -[d] <path>\n");
	fprintf(stderr, "  -d: list contents first before depth traversal\n");
	exit(2);
}

int
main(int argc, char *argv[])
{
	const char *dir;
	int c, flags;

	flags = 0;
	while ((c = getopt(argc, argv, "dh")) > 0) {
		switch (c) {
		case 'h':
			usage();
			break;
		case 'd':
			flags |= FTW_DEPTH;
			break;
		}
	}
	argc -= optind;
	argv += optind;

	dir = ".";
	if (argc >= 1)
		dir = argv[0];

	nftw(dir, list, 10, flags);
	return 0;
}
