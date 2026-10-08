#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

const char *
attrstr(int type, int attr)
{
	switch (type) {
	case 'd':
		switch (attr) {
		case PTHREAD_CREATE_DETACHED:
			return "PTHREAD_CREATE_DETACHED";
		case PTHREAD_CREATE_JOINABLE:
			return "PTHREAD_CREATE_JOINABLE";
		}
		break;

	case 'i':
		switch (attr) {
		case PTHREAD_INHERIT_SCHED:
			return "PTHREAD_INHERIT_SCHED";
		case PTHREAD_EXPLICIT_SCHED:
			return "PTHREAD_EXPLICIT_SCHED";
		}
		break;

	case 'p':
		switch (attr) {
		case SCHED_OTHER:
			return "SCHED_OTHER";
		case SCHED_FIFO:
			return "SCHED_FIFO";
		case SCHED_RR:
			return "SCHED_RR";
		}
		break;

	case 's':
		switch (attr) {
		case PTHREAD_SCOPE_SYSTEM:
			return "PTHREAD_SCOPE_SYSTEM";
		case PTHREAD_SCOPE_PROCESS:
			return "PTHREAD_SCOPE_PROCESS";
		}
		break;
	}
	return "???";
}

void *
attrinfo(void *arg)
{
	pthread_mutex_t *mutex;
	pthread_attr_t attr;
	void *stackaddr;
	size_t stacksize, guardsize;
	int detach, scope, isched, psched;
	struct sched_param sp;

	pthread_attr_init(&attr);
	pthread_attr_getstack(&attr, &stackaddr, &stacksize);
	pthread_attr_getstacksize(&attr, &stacksize);
	pthread_attr_getdetachstate(&attr, &detach);
	pthread_attr_getscope(&attr, &scope);
	pthread_attr_getinheritsched(&attr, &isched);
	pthread_attr_getschedpolicy(&attr, &psched);
	pthread_attr_getguardsize(&attr, &guardsize);
	pthread_attr_getschedparam(&attr, &sp);

	mutex = arg;
	pthread_mutex_lock(mutex);
	printf("thread id: %lx\n", (unsigned long)pthread_self());
	printf("thread stack addr: %p\n", stackaddr);
	printf("thread stack size: %zu bytes\n", stacksize);
	printf("thread guard size: %zu bytes\n", guardsize);
	printf("detach state: %s\n", attrstr('d', detach));
	printf("scope: %s\n", attrstr('s', scope));
	printf("inherit sched: %s\n", attrstr('i', isched));
	printf("sched policy: %s\n", attrstr('p', psched));
	printf("thread priority: %d\n", sp.sched_priority);
	printf("\n");
	pthread_mutex_unlock(mutex);

	return NULL;
}

void *
printargs(void *args)
{
	void **l;
	int *a;
	float *b;
	char *c;
	size_t i, na, nb;

	l = (void **)args;
	a = l[0];
	na = (size_t)(uintptr_t)l[1];
	b = l[2];
	nb = (size_t)(uintptr_t)l[3];
	c = l[4];

	printf("a: ");
	for (i = 0; i < na; i++)
		printf("%d ", a[i]);
	printf("\n");

	printf("b: ");
	for (i = 0; i < nb; i++)
		printf("%.2f ", b[i]);
	printf("\n");

	printf("c: %s\n", c);

	return NULL;
}

int
main(void)
{
	pthread_t thread[4];
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	size_t i;

	attrinfo(&mutex);
	for (i = 0; i < nelem(thread); i++)
		pthread_create(&thread[i], NULL, attrinfo, &mutex);
	for (i = 0; i < nelem(thread); i++)
		pthread_join(thread[i], NULL);

	int a[] = {1, 2, 3, 4, 5};
	float b[] = {3.4, 13.2, 5.2, 1.4, 35.1};
	char c[] = "foo bar baz";
	void *args[] = {a, (void *)(uintptr_t)nelem(a), b, (void *)(uintptr_t)nelem(b), c};
	for (i = 0; i < nelem(thread); i++) {
		pthread_create(&thread[i], NULL, printargs, args);
		pthread_join(thread[i], NULL);
	}

	return 0;
}
