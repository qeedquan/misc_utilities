#include <assert.h>
#include <stdio.h>
#include <threads.h>

typedef unsigned long long uvlong;

typedef struct {
	int id;
	thrd_t thread;
	mtx_t mutex;
	cnd_t cond;
	bool ready;
	uvlong value;
} Worker;

void
initworker(Worker *worker, int id)
{
	assert(mtx_init(&worker->mutex, mtx_plain) == thrd_success);
	assert(cnd_init(&worker->cond) == thrd_success);
	worker->id = id;
	worker->ready = false;
	worker->value = 0;
}

void
notify(Worker *self, Worker *other)
{
	mtx_lock(&other->mutex);
	self->ready = false;
	other->ready = true;
	printf("[%d] Notifying [%d]\n", self->id, other->id);
	cnd_signal(&other->cond);
	mtx_unlock(&other->mutex);
}

void
standby(Worker *self)
{
	mtx_lock(&self->mutex);
	while (!self->ready) {
		printf("[%d] Standby\n", self->id);
		cnd_wait(&self->cond, &self->mutex);
	}
	self->value += 1;
	printf("[%d] Processing value %llu\n", self->id, self->value);
	mtx_unlock(&self->mutex);
}

int
ping(void *arg)
{
	Worker *self, *other;

	self = arg;
	other = self + 1;
	for (;;) {
		notify(self, other);
		standby(self);
	}
	return thrd_success;
}

int
pong(void *arg)
{
	Worker *self, *other;

	other = arg;
	self = other + 1;
	for (;;) {
		standby(self);
		notify(self, other);
	}
	return thrd_success;
}

int
main()
{
	Worker workers[2];
	int i;

	for (i = 0; i < 2; i++)
		initworker(&workers[i], i);

	thrd_create(&workers[0].thread, ping, workers);
	thrd_create(&workers[1].thread, pong, workers);

	for (i = 0; i < 2; i++)
		thrd_join(workers[i].thread, NULL);

	return 0;
}
