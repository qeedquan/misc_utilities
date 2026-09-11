#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <vxWorks.h>
#include <taskLib.h>
#include <semLib.h>

typedef struct {
	SEM_ID mtx;

	TASK_ID mtid;
	TASK_ID *wtid;

	unsigned *cnt;
	unsigned idx;

	int nw;
} Context;

#define lock(x) assert(semTake(x, WAIT_FOREVER) == OK)
#define unlock(x) assert(semGive(x) == OK)

int
monitor(_Vx_usr_arg_t arg1)
{
	Context *c;
	int i;

	c = (void *)arg1;
	for (;;) {
		sleep(1);

		lock(c->mtx);
		syslog(LOG_INFO, " ");
		for (i = 0; i < c->nw; i++)
			printf("%u ", c->cnt[i]);
		printf("\n");
		unlock(c->mtx);
	}
	return 0;
}

int
worker(_Vx_usr_arg_t arg1, _Vx_usr_arg_t arg2)
{
	Context *c;
	unsigned id;

	c = (void *)arg1;
	id = arg2;
	for (;;) {
		lock(c->mtx);
		c->cnt[id]++;
		unlock(c->mtx);
	}
	return 0;
}

Context *
newcontext(int nw, int fair)
{
	Context *c;
	char name[32];
	int prio, opts;
	int i;

	prio = 0;
	opts = 0;

	c = calloc(1, sizeof(*c));
	assert(c != NULL);

	c->cnt = calloc(nw, sizeof(*c->cnt));
	assert(c->cnt != NULL);

	c->mtx = semMCreate(SEM_Q_PRIORITY | SEM_INVERSION_SAFE);
	assert(c->mtx != SEM_ID_NULL);

	c->mtid = taskOpen("monitor", prio, opts, OM_CREATE | OM_EXCL, NULL, 4096, NULL, (FUNCPTR)monitor, (_Vx_usr_arg_t)c, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	assert(c->mtid != TASK_ID_NULL);

	c->wtid = calloc(nw, sizeof(*c->wtid));
	assert(c->wtid != NULL);
	for (i = 0; i < nw; i++) {
		snprintf(name, sizeof(name), "worker-%d", i);
		c->wtid[i] = taskOpen(name, (fair) ? prio : prio + i + 1, opts, OM_CREATE | OM_EXCL, NULL, 4096, NULL, (FUNCPTR)worker, (_Vx_usr_arg_t)c, i, 0, 0, 0, 0, 0, 0, 0, 0);
		assert(c->wtid[i] != TASK_ID_NULL);
	}

	c->nw = nw;

	return c;
}

void
runcontext(Context *c)
{
	int i;

	assert(taskActivate(c->mtid) == OK);
	for (i = 0; i < c->nw; i++)
		assert(taskActivate(c->wtid[i]) == OK);
	taskWait(c->mtid, WAIT_FOREVER);
}

int
main(int argc, char *argv[])
{
	int nw;
	int fair;

	nw = 50;
	fair = 0;
	if (argc >= 2)
		nw = atoi(argv[1]);
	if (argc >= 3)
		fair = atoi(argv[2]);
	assert(nw >= 1);

	runcontext(newcontext(nw, fair));
	return 0;
}
