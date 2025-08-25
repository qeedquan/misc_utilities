#include <vxWorks.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>
#include <taskLib.h>

int
reader(_Vx_usr_arg_t fd, _Vx_usr_arg_t bufsz)
{
	struct timespec t0, t1;
	char *buf;
	int64_t tot, nr;

	buf = calloc(1, bufsz);
	if (!buf)
		return ERROR;

	clock_gettime(CLOCK_MONOTONIC, &t0);

	tot = 0;
	for (;;) {
		nr = read(fd, buf, bufsz);
		if (nr < 0) {
			perror("read");
			continue;
		}

		clock_gettime(CLOCK_MONOTONIC, &t1);
		if (t1.tv_sec != t0.tv_sec) {
			t0 = t1;
			printf("Read %" PRId64 " bytes (%" PRId64 " megabytes)\n", tot, tot / (1024 * 1024));
		}
		tot += nr;
	}

	free(buf);
	return OK;
}

int
writer(_Vx_usr_arg_t fd, _Vx_usr_arg_t bufsz)
{
	char *buf;

	buf = calloc(1, bufsz);
	if (!buf)
		return ERROR;

	for (;;) {
		write(fd, buf, bufsz);
	}

	free(buf);
	return OK;
}

int
main(int argc, char *argv[])
{
	int tid[2];
	int fd[2];
	int prio, opts;
	int bufsz;
	int i;

	bufsz = 8192;
	if (argc >= 2)
		bufsz = atoi(argv[1]);
	assert(bufsz >= 1);

	if (pipe(fd) < 0) {
		perror("pipe");
		return 1;
	}

	prio = 100;
	opts = VX_FP_TASK;
	tid[0] = taskSpawn("posix-pipe-reader", prio, opts, 128 * 1024, (FUNCPTR)reader, fd[0], bufsz, 0, 0, 0, 0, 0, 0, 0, 0);
	tid[1] = taskSpawn("posix-pipe-writer", prio, opts, 128 * 1024, (FUNCPTR)writer, fd[1], bufsz, 0, 0, 0, 0, 0, 0, 0, 0);

	for (i = 0; i < 2; i++)
		taskWait(tid[i], WAIT_FOREVER);

	return 0;
}
