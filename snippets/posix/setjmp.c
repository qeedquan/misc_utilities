#define _GNU_SOURCE
#include <stdio.h>
#include <setjmp.h>
#include <signal.h>

sigjmp_buf jmpbuf;

void
handle(int sig)
{
	siglongjmp(jmpbuf, 1);
	(void)sig;
}

int
main(void)
{
	char *p;
	int i;

	p = NULL;
	i = 0;

	signal(SIGSEGV, handle);
	sigsetjmp(jmpbuf, 1);

	for (;;) {
		printf("%d\n", i++);
		*p = 1;
	}
	return 0;
}
