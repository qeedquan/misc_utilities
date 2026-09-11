#include <stdio.h>

#define SECTION(x) __attribute__((section(x)))
#define MSABI __attribute__((ms_abi))
#define SYSV __attribute__((sysv_abi))

SECTION(".foo")
MSABI int foo(int, int, int);

SECTION(".bar")
SYSV const char *bar(const char *);

MSABI int
foo(int x, int y, int z)
{
	printf("%d %d %d\n", x, y, z);
	return x + y + z;
}

SYSV const char *
bar(const char *s)
{
	return s;
}

int
main(void)
{
	foo(5, 6, 7);
	printf("%s\n", bar("hello"));
	return 0;
}
