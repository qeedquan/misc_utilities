#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t cond;

void
sigalarm(int sig)
{
	cond = 1;
	(void)sig;
}

void
strsig(void)
{
	int i;
	char *s;

	for (i = 0; i < 1024; i++) {
		s = strsignal(i);
		if (!s || strncasecmp(s, "Unknown", 7) == 0)
			continue;

		printf("%d %s\n", i, s);
	}
}

void
sirens(void)
{
	int i;

	signal(SIGALRM, sigalarm);
	cond = 0;
	for (i = 0; i <= 10; i++) {
		alarm(1);

		while (!cond)
			;

		printf("%d\n", i);
		cond = 0;
	}
}

int
main(void)
{
	strsig();
	sirens();
	return 0;
}
