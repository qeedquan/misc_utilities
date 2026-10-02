#include <stdio.h>
#include <time.h>

void
hello(void)
{
	long long t;

	t = time(NULL);
	printf("%lld: hello\n", t);
}
