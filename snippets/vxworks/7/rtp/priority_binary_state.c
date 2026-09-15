#include <vxWorks.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <syslog.h>
#include <sysLib.h>
#include <clockLib.h>
#include <taskLib.h>
#include <semLib.h>

typedef struct {
	TASK_ID ttid;
	TASK_ID rtid;

	SEM_ID mtx;
	int state;
	int prio;

	long long rate;

	unsigned cnt[2];
} Context;

#define lock(x) assert(semTake(x, WAIT_FOREVER) == OK)
#define unlock(x) assert(semGive(x) == OK)

void
millisleep(long long ms)
{
	struct timespec req = {
	    ms / 1000ll,
	    (ms % 1000) * 1000000ll,
	};
	assert(clock_nanosleep(CLOCK_MONOTONIC, 0, &req, NULL) == OK);
}

long long
timesub(struct timespec *a, struct timespec *b)
{
	long long t0, t1;

	t1 = (a->tv_sec * 1000ll) + (a->tv_nsec / 1000000ll);
	t0 = (b->tv_sec * 1000ll) + (b->tv_nsec / 1000000ll);
	return t1 - t0;
}

void
transition(Context *c, int state, int prio)
{
	lock(c->mtx);
	if (prio >= c->prio) {
		c->state = state;
		c->prio = prio;
	}
	unlock(c->mtx);
}

int
transitioner(_Vx_usr_arg_t arg1)
{
	Context *c;
	struct timespec t0, t1;
	int state;

	c = (void *)arg1;

	clock_gettime(CLOCK_MONOTONIC, &t0);
	for (;;) {
		lock(c->mtx);
		state = c->state;
		c->prio = 0;
		unlock(c->mtx);

		clock_gettime(CLOCK_MONOTONIC, &t1);
		if (timesub(&t1, &t0) >= 1000) {
			syslog(LOG_INFO, "%u %u", c->cnt[0], c->cnt[1]);
			t0 = t1;
		}

		if (state < 100) {
			c->cnt[0]++;
			transition(c, (state + 1) % 100, 0);
		} else {
			c->cnt[1]++;
			transition(c, 0, 0);
		}
	}
	return 0;
}

int
requester(_Vx_usr_arg_t arg1)
{
	Context *c;

	c = (void *)arg1;
	for (;;) {
		millisleep(c->rate);
		transition(c, 100, 1);
	}
	return 0;
}

int
main(int argc, char *argv[])
{
	Context c[1];
	int prio, opts;
	long long rate;

	prio = 0;
	opts = 0;
	rate = 100;
	if (argc >= 2)
		rate = atoll(argv[1]);
	assert(rate >= 1);

	memset(c, 0, sizeof(*c));
	c->rate = rate;

	c->mtx = semMCreate(SEM_Q_FIFO);
	assert(c->mtx != SEM_ID_NULL);

	c->ttid = taskSpawn("transitioner", prio, opts, 128 * 1024, (FUNCPTR)transitioner, (_Vx_usr_arg_t)c, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	assert(c->ttid != TASK_ID_NULL);

	c->rtid = taskSpawn("requester", prio, opts, 128 * 1024, (FUNCPTR)requester, (_Vx_usr_arg_t)c, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	assert(c->rtid != TASK_ID_NULL);

	taskWait(c->ttid, WAIT_FOREVER);

	return 0;
}
