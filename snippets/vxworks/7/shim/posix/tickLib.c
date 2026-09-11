#include "vxWorks.h"

typedef struct {
	pthread_mutex_t lock;
	struct timespec start;
	int once;
} Ticker;

static Ticker ticker = {
    .lock = PTHREAD_MUTEX_INITIALIZER,
};

uint64_t
timespec_to_millisecond(struct timespec *tp)
{
	return (tp->tv_sec * 1000) + (tp->tv_nsec / 1000000);
}

struct timespec
millisecond_to_timespec(uint64_t ms)
{
	return (struct timespec){
	    .tv_sec = ms / 1000,
	    .tv_nsec = (ms % 1000) * 1000000,
	};
}

uint64_t
tick_to_millisecond(_Vx_ticks64_t tick)
{
	return (1000 * tick) / tickRateGet();
}

static _Vx_ticks64_t
ticks(void)
{
	struct timespec now;
	Ticker *t;
	uint64_t dt;

	t = &ticker;
	pthread_mutex_lock(&t->lock);
	if (!t->once) {
		clock_gettime(CLOCK_MONOTONIC, &t->start);
		t->once = 1;
	}
	pthread_mutex_unlock(&t->lock);

	clock_gettime(CLOCK_MONOTONIC, &now);

	dt = timespec_to_millisecond(&now) - timespec_to_millisecond(&t->start);
	return (dt * tickRateGet()) / 1000;
}

_Vx_ticks_t
tickGet(void)
{
	return ticks();
}

_Vx_ticks64_t
tick64Get(void)
{
	return ticks();
}

_Vx_freq_t
tickRateGet(void)
{
	return VX_TICK_RATE;
}
