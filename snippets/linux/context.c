#include <stdio.h>
#include <ucontext.h>
#include <unistd.h>

int
main(void)
{
	ucontext_t context;
	int i;

	i = 0;
	getcontext(&context);
	printf("%d\n", i++);
	sleep(1);
	setcontext(&context);
	return 0;
}
