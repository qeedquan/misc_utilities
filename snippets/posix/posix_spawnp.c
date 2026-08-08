#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <spawn.h>
#include <wait.h>
#include <unistd.h>
#include <pthread.h>
#include <err.h>

extern char **environ;

void
usage(void)
{
	fprintf(stderr, "usage: command args...\n");
	exit(2);
}

void *
reader(void *ud)
{
	int *pfd;
	char buf[BUFSIZ];
	ssize_t nr;

	pfd = ud;
	while ((nr = read(pfd[0], buf, sizeof(buf) - 1)) > 0) {
		buf[nr] = '\0';
		printf("***READER\n");
		printf("%s", buf);
	}
	return NULL;
}

int
main(int argc, char *argv[])
{
	pthread_t thread;
	posix_spawn_file_actions_t fa;
	posix_spawnattr_t attr;
	pid_t pid;
	int pfd[2];
	int status;
	int r;

	if (argc < 2)
		usage();

	pipe(pfd);
	posix_spawn_file_actions_init(&fa);
	posix_spawn_file_actions_addclose(&fa, pfd[0]);
	posix_spawn_file_actions_addclose(&fa, pfd[0]);
	posix_spawn_file_actions_adddup2(&fa, pfd[1], 1);
	posix_spawn_file_actions_adddup2(&fa, pfd[1], 2);

	posix_spawnattr_init(&attr);

	r = posix_spawnp(&pid, argv[1], &fa, &attr, argv + 1, environ);
	if (r < 0)
		errx(1, "posix_spawn: %s", strerror(r));

	close(pfd[1]);

	pthread_create(&thread, NULL, reader, pfd);
	pthread_join(thread, NULL);

	r = waitpid(pid, &status, WUNTRACED);
	printf("\nChild exited with %d\n", r);

	posix_spawn_file_actions_destroy(&fa);
	posix_spawnattr_destroy(&attr);

	return 0;
}
