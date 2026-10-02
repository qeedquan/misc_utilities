#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <time.h>
#include <unistd.h>
#include <vxWorks.h>
#include <taskLib.h>
#include <semLib.h>

typedef struct {
	SEM_ID mtx;
	unsigned cnt;
	atomic_uint done;
} Context;

int
runner(_Vx_usr_arg_t arg1, _Vx_usr_arg_t arg2)
{
	Context *c;
	long id;

	c = (void *)arg1;
	id = arg2;
	while (!c->done) {
		assert(semTake(c->mtx, WAIT_FOREVER) == OK);
		printf("%ld: %ld: %u\n", (long)time(NULL), id, c->cnt);
		c->cnt += id;
		assert(semGive(c->mtx) == OK);
	}
	return 0;
}

int
main(int argc, char *argv[])
{
	Context ctx;
	TASK_ID *tid;
	char name[32];
	int prio, opts;
	int ntasks;
	int secs;
	int i;

	prio = 0;
	opts = 0;
	ntasks = 100;
	secs = 10;
	if (argc >= 2)
		ntasks = atoi(argv[1]);
	if (argc >= 3)
		secs = atoi(argv[2]);

	assert(ntasks >= 1);
	assert(secs >= 1);

	memset(&ctx, 0, sizeof(ctx));
	ctx.mtx = semMCreate(SEM_Q_FIFO);
	assert(ctx.mtx != SEM_ID_NULL);

	tid = calloc(ntasks, sizeof(*tid));
	assert(tid != NULL);
	for (i = 0; i < ntasks; i++) {
		snprintf(name, sizeof(name), "runner-%d", i + 1);
		tid[i] = taskSpawn(name, prio, opts, 128 * 1024, (FUNCPTR)runner, (_Vx_usr_arg_t)&ctx, i + 1, 0, 0, 0, 0, 0, 0, 0, 0);
		assert(tid[i] != TASK_ID_NULL);
	}

	sleep(secs);
	ctx.done = 1;
	for (i = 0; i < ntasks; i++)
		taskWait(tid[i], WAIT_FOREVER);
	printf("%ld: Finished at %u\n", (long)time(NULL), ctx.cnt);

	return 0;
}
