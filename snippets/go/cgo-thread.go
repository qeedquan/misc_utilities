package main

/*
#include <stdio.h>
#include <inttypes.h>
#include <pthread.h>
#include <threads.h>

void *runner(void *u) {
	static thread_local int x;
	struct timespec tp;
	time_t s;
	long ms;

	x = pthread_self();
	for (;;) {
		clock_gettime(CLOCK_REALTIME, &tp);
		s = tp.tv_sec;
		ms = tp.tv_nsec / 1000000ul;
		printf("%#jx %jd.%ld %d\n", (uintmax_t)pthread_self(), (intmax_t)s, ms, x);
	}
	return NULL;
}

void runthread(void) {
	pthread_t tid;
	pthread_create(&tid, NULL, runner, NULL);
	pthread_detach(tid);
}
*/
import "C"

func main() {
	for i := 0; i < 32; i++ {
		C.runthread()
	}
	select {}
}
