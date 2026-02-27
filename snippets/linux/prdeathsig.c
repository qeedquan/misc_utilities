#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <sys/prctl.h>
#include <unistd.h>
#include <err.h>

volatile sig_atomic_t done;

void
sigusr1(int sig)
{
	done = 1;
	(void)sig;
}

int
main(void)
{
	time_t t;

	srand(time(NULL));
	switch (fork()) {
	case -1:
		err(1, "fork");
		break;
	case 0:
		if (signal(SIGUSR1, sigusr1) == SIG_ERR)
			err(1, "signal");
		if (prctl(PR_SET_PDEATHSIG, SIGUSR1) < 0)
			err(1, "prctl");

		t = time(NULL);
		while (!done)
			sleep(1);
		printf("parent died in %lu seconds\n\n", time(NULL) - t);
		break;
	default:
		sleep(rand() % 8);
		break;
	}
	return 0;
}
