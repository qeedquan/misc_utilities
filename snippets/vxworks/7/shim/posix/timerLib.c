#include "vxWorks.h"

#undef timer_create

typedef struct {
	timer_t id;
	struct sigevent sev;
	TIMER_CONNECT_FUNC func;
	_Vx_usr_arg_t arg;
} Timer;

static Timer timers[128];
static size_t ntimer;
static pthread_mutex_t tmlk = PTHREAD_MUTEX_INITIALIZER;

static void
timerlock(void)
{
	pthread_mutex_lock(&tmlk);
}

static void
timerunlock(void)
{
	pthread_mutex_unlock(&tmlk);
}

static Timer *
timerfind(timer_t id)
{
	Timer *t;
	size_t i;

	t = NULL;
	for (i = 0; i < ntimer; i++) {
		if (id == timers[i].id) {
			t = timers + i;
			break;
		}
	}
	return t;
}

static void
timerhandle(int sig, siginfo_t *info, void *context)
{
	Timer *t;

	t = info->si_value.sival_ptr;
	t->func(t->id, t->arg);
}

int
vxtimer_create(clockid_t clockid, struct sigevent *sevp, timer_t *timerid)
{
	struct sigevent sev;
	Timer *t;
	int r;

	r = 0;
	timerlock();
	if (ntimer >= nelem(timers)) {
		r = ERROR;
		errno = EINVAL;
		goto out;
	}

	t = timers + ntimer;
	memset(&sev, 0, sizeof(sev));
	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_signo = SIGALRM;
	if (sevp)
		sev = *sevp;
	sev.sigev_value.sival_ptr = t;

	r = timer_create(clockid, &sev, timerid);
	if (r < 0)
		goto out;

	t->id = *timerid;
	t->sev = sev;
	ntimer++;

out:
	timerunlock();
	return r;
}

int
timer_connect(timer_t timerid, TIMER_CONNECT_FUNC routine, _Vx_usr_arg_t arg)
{
	struct sigaction sa;
	Timer *t;
	int r;

	r = 0;
	timerlock();
	t = timerfind(timerid);
	if (!t) {
		r = ERROR;
		errno = EINVAL;
		goto out;
	}

	t->func = routine;
	t->arg = arg;

	memset(&sa, 0, sizeof(sa));
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = timerhandle;
	sigemptyset(&sa.sa_mask);
	r = sigaction(t->sev.sigev_signo, &sa, NULL);

out:
	timerunlock();
	return r;
}
