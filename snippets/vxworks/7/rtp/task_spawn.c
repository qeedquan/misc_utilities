#include <vxWorks.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <time.h>
#include <unistd.h>
#include <taskLib.h>
#include <semLib.h>
#include "util.h"

typedef struct {
	atomic_uint shared;
} Context;

char *
timestamp(char *buf)
{
	struct tm tm;
	time_t t;
	char *p;

	time(&t);
	gmtime_r(&t, &tm);
	asctime_r(&tm, buf);
	p = strchr(buf, '\n');
	if (p)
		*p = '\0';
	return buf;
}

int
runner(_Vx_usr_arg_t arg1, _Vx_usr_arg_t arg2)
{
	Context *c;
	long id;
	unsigned val;
	char buf[32];

	c = (void *)arg1;
	id = arg2;

	while (c->shared == 0)
		taskDelay(50);

	for (;;) {
		val = c->shared++;
		xprintf("%s: Task %ld: value %u\n", timestamp(buf), id, val);
		sleep((rand() % 10) + 2);
	}
	return 0;
}

int
main(int argc, char *argv[])
{
	TASK_ID *tid;
	char name[32];
	Context ctx;
	int ntasks;
	int prio, opts;
	int i;

	initutil();

	ntasks = 50;
	prio = 100;
	opts = VX_FP_TASK;
	if (argc >= 2)
		ntasks = atoi(argv[1]);

	srand(time(NULL));
	tid = calloc(ntasks, sizeof(*tid));
	assert(tid != NULL);

	memset(&ctx, 0, sizeof(ctx));
	for (i = 0; i < ntasks; i++) {
		snprintf(name, sizeof(name), "taskrun-test-%d", i + 1);
		xprintf("Starting %s\n", name);

		tid[i] = taskSpawn(name, prio, opts, 128 * 1024, (FUNCPTR)runner, (_Vx_usr_arg_t)&ctx, i + 1, 0, 0, 0, 0, 0, 0, 0, 0);
		assert(tid[i] != TASK_ID_NULL);
	}
	ctx.shared = 1;

	for (i = 0; i < ntasks; i++)
		taskWait(tid[i], WAIT_FOREVER);

	return 0;
}
