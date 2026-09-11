#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

_Thread_local char tgbuf[1024];

void *
foo(void *arg)
{
	// thread local static will have a different address
	// on each thread, but the static have the same address
	// for all threads
	_Thread_local static char tbuf[1024];
	static char buf[1024];
	char str[128];
	int n;

	n = snprintf(str, sizeof(str), "%p %p %p\n", buf, tbuf, tgbuf);
	write(1, str, n);
	(void)arg;
	return NULL;
}

int
main(void)
{
	pthread_t thread[8];
	size_t i;

	for (i = 0; i < nelem(thread); i++)
		pthread_create(&thread[i], NULL, foo, NULL);
	for (i = 0; i < nelem(thread); i++)
		pthread_join(thread[i], NULL);

	return 0;
}
