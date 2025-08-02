#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>

char *
xstrdup(const char *s)
{
	char *p;

	p = strdup(s);
	if (!p)
		abort();
	return p;
}

void
test_strsep(char **text, const char *delim, bool dofree)
{
	char *ptr, *str;

	ptr = *text;
	while ((str = strsep(text, delim)))
		printf("%s\n", str);
	if (dofree)
		free(ptr);
}

int
main(void)
{
	char *s1 = xstrdup("hello world");
	test_strsep(&s1, " ", true);

	char s2[] = "stack allocated";
	char *p2 = s2;
	test_strsep(&p2, " ", false);

	char *s3 = xstrdup("1,2,3,4,5,6,7,8,9,10");
	test_strsep(&s3, ",", true);

	printf("%jd\n", strtoimax("1023", NULL, 0));
	printf("%jd\n", strtoimax("0x24", NULL, 0));
	printf("%jd\n", strtoimax("101", NULL, 2));
	printf("%jd\n", imaxabs(-20));

	return 0;
}
