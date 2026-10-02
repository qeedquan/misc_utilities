#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>
#include <err.h>

int
main(void)
{
	sem_t *sem;

	sem = sem_open("/foo", O_CREAT, 0644, 1);
	if (sem == SEM_FAILED)
		err(1, "sem_open");

	for (;;) {
		sem_wait(sem);
		printf("%ld: obtained\n", (long)getpid());
		sem_post(sem);
		sleep(1);
	}

	sem_close(sem);
	return 0;
}
