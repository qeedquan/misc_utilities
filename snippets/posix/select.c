#include <stdio.h>
#include <sys/select.h>

#define P(x) printf("%s: %d\n", #x, x);

void
limits(void)
{
	// max file descriptor set that select can support
	P(FD_SETSIZE);
}

int
main(void)
{
	limits();
	return 0;
}
