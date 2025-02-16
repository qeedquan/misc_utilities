#include <vxWorks.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <taskLib.h>
#include <sysLib.h>
#include <semLib.h>
#include "util.h"

void
take(SEM_ID id)
{
	int r;

	r = semTake(id, WAIT_FOREVER);
	if (r != OK) {
		xprintf("Error taking semaphore: %s\n", strerror(errno));
		assert(0);
	}
}

void
give(SEM_ID id)
{
	int r;

	r = semGive(id);
	if (r != OK) {
		xprintf("Error release semaphore: %s\n", strerror(errno));
		assert(0);
	}
}

void
ticksleep(long secs)
{
	_Vx_freq_t rate;
	long i;

	rate = sysClkRateGet();
	for (i = 0; i < secs; i++)
		taskDelay(rate);
}

int
runner(_Vx_usr_arg_t arg1, _Vx_usr_arg_t arg2, _Vx_usr_arg_t arg3, _Vx_usr_arg_t arg4)
{
	struct timespec tp;
	SEM_ID *sem;
	long id, nid;
	long secs;

	sem = (void *)arg1;
	id = arg2;
	nid = arg3;
	secs = arg4;

	take(sem[id]);
	ticksleep(secs);
	if (id + 1 < nid)
		give(sem[id + 1]);
	give(sem[id]);

	clock_gettime(CLOCK_MONOTONIC, &tp);
	xprintf("%ld.%ld: Task %d finished\n", (long)tp.tv_sec, tp.tv_nsec, id);
	return 0;
}

int
main(int argc, char *argv[])
{
	TASK_ID *tid;
	SEM_ID *sem;
	char name[32];
	int ntasks;
	int prio, opts;
	int secs;
	int i;

	initutil();

	ntasks = 10;
	prio = 100;
	opts = 0;
	secs = 1;
	if (argc >= 2)
		ntasks = atoi(argv[1]);
	if (argc >= 3)
		secs = atoi(argv[2]);

	assert(ntasks >= 1);
	assert(secs >= 1);

	tid = calloc(ntasks, sizeof(*tid));
	assert(tid != NULL);

	sem = calloc(ntasks, sizeof(*sem));
	assert(sem != NULL);

	for (i = 0; i < ntasks; i++) {
		sem[i] = semBCreate(SEM_Q_FIFO, SEM_EMPTY);
		assert(sem[i] != SEM_ID_NULL);
	}

	for (i = 0; i < ntasks; i++) {
		snprintf(name, sizeof(name), "bsem-test-%d", i);
		xprintf("Starting %s\n", name);

		tid[i] = taskSpawn(name, prio, opts, 128 * 1024, (FUNCPTR)runner, (_Vx_usr_arg_t)sem, i, ntasks, secs, 0, 0, 0, 0, 0, 0);
		assert(tid[i] != TASK_ID_NULL);
	}

	ticksleep(secs);
	give(sem[0]);

	for (i = 0; i < ntasks; i++)
		taskWait(tid[i], WAIT_FOREVER);
	for (i = 0; i < ntasks; i++)
		semDelete(sem[i]);

	free(tid);
	free(sem);

	return 0;
}
