#define _GNU_SOURCE
#include <stdio.h>
#include <libgen.h>
#include <limits.h>
#include <unistd.h>

void
test(const char *path)
{
	char dir[PATH_MAX], base[PATH_MAX];

	snprintf(dir, sizeof(dir), "%s", path);
	snprintf(base, sizeof(base), "%s", path);
	printf("path: %s\n", path);
	printf("dir:  %s\n", dirname(dir));
	printf("base: %s\n", basename(base));
	printf("\n");
}

int
main(void)
{
	test("C:\\Windows\\System and stuff/bah, foo, baz/kk");
	test("");
	test(".");
	test("/etc/group-by-things/1rf/a/f/b/e/2/xx");
	return 0;
}
