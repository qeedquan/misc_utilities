#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/statfs.h>
#include <dirent.h>

char *
strsize(char *buf, size_t len, double val)
{
	static const double multiplier[] = {
	    1,
	    1e3,
	    1e6,
	    1e9,
	    1e12,
	};

	static const char *units[] = {
	    "",
	    "K",
	    "M",
	    "G",
	    "T",
	};

	size_t i;

	for (i = NELEMENTS(multiplier); i > 0; i--) {
		if (val >= multiplier[i - 1]) {
			snprintf(buf, len, "%.2f%sB", val / multiplier[i - 1], units[i - 1]);
			break;
		}
	}

	return buf;
}

void
df(const char *top)
{
	char path[1024], buf[1024], *canon;
	struct statfs64 fs;
	struct dirent *de;
	DIR *d;

	d = opendir(top);
	if (!d) {
		fprintf(stderr, "opendir: %s\n", strerror(errno));
		return;
	}

	printf("Directory: %s\n", top);
	printf("%-32s %-16s %-16s %-16s\n", "Path", "Size", "Free", "Usage");
	while ((de = readdir(d))) {
		if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."))
			continue;

		snprintf(path, sizeof(path), "%s/%s", top, de->d_name);
		if (statfs64(path, &fs) < 0)
			continue;

		canon = realpath(path, NULL);
		if (!canon)
			canon = path;

		printf("%-32s ", canon);
		printf("%-16s ", strsize(buf, sizeof(buf), fs.f_bsize * fs.f_blocks));
		printf("%-16s ", strsize(buf, sizeof(buf), fs.f_bsize * fs.f_bfree));
		snprintf(buf, sizeof(buf), "%.2f%%", 100.0 - (fs.f_bfree * 100.0 / fs.f_blocks));
		printf("%-16s ", buf);
		printf("\n");

		if (canon != path)
			free(canon);
	}
	printf("\n");

	closedir(d);
}

void
usage(void)
{
	fprintf(stderr, "usage: <dir> ...\n");
	exit(2);
}

int
main(int argc, char *argv[])
{
	int i;

	if (argc < 2)
		usage();

	for (i = 1; i < argc; i++)
		df(argv[i]);

	return 0;
}
