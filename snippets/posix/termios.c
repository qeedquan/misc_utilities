#define _GNU_SOURCE
#include <stdio.h>
#include <termios.h>

#define P(x) printf("%s: %#x\n", #x, x)

void
defs(void)
{
	P(CSTOPB);
	P(PARENB);
	P(CS5);
	P(CS6);
	P(CS7);
	P(CS8);
}

int
main(void)
{
	defs();
	return 0;
}
