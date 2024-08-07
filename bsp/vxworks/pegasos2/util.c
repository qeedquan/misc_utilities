#include "dat.h"
#include "fns.h"

time_t time0;

void
utilinit(void)
{
	time0 = time(NULL);
}

time_t
uptime(void)
{
	time_t t;

	t = time(NULL) - time0;
	printf("%ld\n", (long)t);
	return t;
}

void
utillink(void)
{
}
