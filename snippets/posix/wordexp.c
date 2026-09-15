#include <stdio.h>
#include <wordexp.h>

void
test(const char *s, int flags)
{
	wordexp_t p;
	size_t i;

	wordexp(s, &p, flags);
	printf("%s ->\n", s);
	for (i = 0; i < p.we_wordc; i++) {
		printf("\t%s\n", p.we_wordv[i]);
	}
	wordfree(&p);
	printf("\n");
}

int
main(void)
{
	test("~", 0);
	test("~root", 0);
	test("$HOME", 0);
	test("$PATH", 0);
	test("#abc", 0);
	test("/home/*", 0);
	test("/etc/\?\?\?\?\?\?/", 0);
	return 0;
}
