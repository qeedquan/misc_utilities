#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

#define min(a, b) (((a) < (b)) ? (a) : (b))

int
snprint(char *str, size_t size, const char *fmt, ...)
{
	va_list ap;
	size_t n;

	va_start(ap, fmt);
	n = vsnprintf(str, size, fmt, ap);
	va_end(ap);
	return min(size, n);
}

void
usage(void)
{
	fprintf(stderr, "usage: cmd ...\n");
	exit(2);
}

int
main(int argc, char *argv[])
{
	char buf[256];
	int i, r, len;

	if (argc < 2)
		usage();

	len = 0;
	for (i = 1; i < argc; i++) {
		len += snprint(buf + len, sizeof(buf) - len, "%s", argv[i]);
		if (i + 1 < argc)
			len += snprint(buf + len, sizeof(buf) - len, " ");
	}

	printf("Executing '%s'\n", buf);
	r = system(buf);
	printf("Return code = %d\n", r);
	if (r != 0)
		printf("Error: %s\n", strerror(errno));

	return 0;
}
