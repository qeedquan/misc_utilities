#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void
handler(int sig)
{
	const char str[] = "got sigill\n";

	write(1, str, sizeof(str) - 1);
	(void)sig;
}

int
main(void)
{
	signal(SIGILL, handler);
	__builtin_trap();
	printf("got here\n");
	return 0;
}
