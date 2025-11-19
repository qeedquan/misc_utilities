#include "vxWorks.h"

typedef struct {
	pthread_mutex_t mutex;
} SD;

static pthread_mutex_t sdlk = PTHREAD_MUTEX_INITIALIZER;
static SD sds[128];
static size_t nsd;

static void
sdlock(void)
{
	pthread_mutex_lock(&sdlk);
}

static void
sdunlock(void)
{
	pthread_mutex_unlock(&sdlk);
}

SEM_ID
semBCreate(int options, SEM_B_STATE initialState)
{
	SEM_ID ret;
	SD *sd;

	ret = SEM_ID_NULL;
	sdlock();
	if (nsd >= nelem(sds))
		goto out;

	sd = &sds[++nsd];
	pthread_mutex_init(&sd->mutex, NULL);
	if (initialState == SEM_EMPTY)
		pthread_mutex_lock(&sd->mutex);

	ret = sd;

out:
	sdunlock();
	return ret;
}

SEM_ID
semMCreate(int options)
{
	return semBCreate(options, SEM_FULL);
}

STATUS
semTake(SEM_ID semId, _Vx_ticks_t timeout)
{
	struct timespec tp;
	long rate;
	SD *sd;
	int r;
	int id;

	sdlock();
	id = (uintptr_t)semId;
	if (id < 0 || id > nsd) {
		sdunlock();
		return ERROR;
	}
	sd = sds + id;
	sdunlock();

	if (timeout == WAIT_FOREVER)
		pthread_mutex_lock(&sd->mutex);
	else {
		clock_gettime(CLOCK_REALTIME, &tp);
		rate = sysClkRateGet();
		tp.tv_sec += timeout / rate;
		tp.tv_nsec += ((timeout % rate) * 100000000L) / rate;

		r = pthread_mutex_timedlock(&sd->mutex, &tp);
		if (r != 0) {
			errno = r;
			return ERROR;
		}
	}

	return OK;
}

STATUS
semGive(SEM_ID semId)
{
	SD *sd;

	sdlock();
	sd = semId;
	if (sd == NULL) {
		sdunlock();
		return ERROR;
	}
	sdunlock();

	pthread_mutex_unlock(&sd->mutex);
	return OK;
}

STATUS
semDelete(SEM_ID semId)
{
	return OK;
}

STATUS
semShow(SEM_ID semId, int level)
{
	return OK;
}

STATUS
semMInit(SEMAPHORE *semaphore, int options)
{
	return OK;
}
