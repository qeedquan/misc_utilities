// https://en.wikipedia.org/wiki/Peterson%27s_algorithm

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <inttypes.h>
#include <threads.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

typedef struct {
	volatile atomic_bool flag[2];
	volatile atomic_int turn;
} Lock;

typedef struct {
	Lock *lock;
	int id;
} Context;

void
peterson(Lock *l, int n, void (*f)(void *), void *u)
{
	l->flag[n] = true;
	l->turn = n;
	while (l->flag[1 - n] && l->turn == n)
		;
	f(u);
	l->flag[n] = false;
}

void
print(void *u)
{
	Context *c;

	c = u;
	printf("hello from %d\n", c->id);
	fflush(stdout);
	thrd_sleep(&(struct timespec){ .tv_sec = 1 }, NULL);
}

int
thread(void *u)
{
	Context *c;

	c = u;
	for (;;) {
		peterson(c->lock, c->id, print, c);
	}
	return 0;
}

int
main(void)
{
	thrd_t thr[2];
	Context c[2];
	Lock l;
	size_t i;

	memset(&c, 0, sizeof(c));
	memset(&l, 0, sizeof(l));

	for (i = 0; i < nelem(thr); i++) {
		c[i].lock = &l;
		c[i].id = i;
		thrd_create(&thr[i], thread, &c[i]);
	}
	for (i = 0; i < nelem(thr); i++)
		thrd_join(thr[i], NULL);

	return 0;
}
