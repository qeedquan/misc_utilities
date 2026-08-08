#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>

typedef struct {
	pthread_mutex_t mutex;
	pthread_cond_t cond_producer;
	pthread_cond_t cond_consumer;
	bool can_produce;
	bool can_consume;
	int counter;
} ctx_t;

void
init_ctx(ctx_t *ctx)
{
	pthread_mutex_init(&ctx->mutex, NULL);
	pthread_cond_init(&ctx->cond_producer, NULL);
	pthread_cond_init(&ctx->cond_consumer, NULL);
	ctx->can_produce = true;
	ctx->can_consume = false;
	ctx->counter = 0;
}

void *
producer(void *arg)
{
	ctx_t *ctx = arg;
	for (;;) {
		pthread_mutex_lock(&ctx->mutex);
		while (!ctx->can_produce)
			pthread_cond_wait(&ctx->cond_producer, &ctx->mutex);
		ctx->counter++;
		ctx->can_consume = true;
		ctx->can_produce = false;
		pthread_cond_signal(&ctx->cond_consumer);
		pthread_mutex_unlock(&ctx->mutex);
	}
	return NULL;
}

void *
consumer(void *arg)
{
	ctx_t *ctx = arg;
	for (;;) {
		pthread_mutex_lock(&ctx->mutex);
		while (!ctx->can_consume)
			pthread_cond_wait(&ctx->cond_consumer, &ctx->mutex);
		printf("%d\n", ctx->counter);
		ctx->can_consume = false;
		ctx->can_produce = true;
		pthread_cond_signal(&ctx->cond_producer);
		pthread_mutex_unlock(&ctx->mutex);
	}
	return NULL;
}

int
main(void)
{
	pthread_t thread[2];
	ctx_t ctx;

	init_ctx(&ctx);
	pthread_create(&thread[0], NULL, producer, &ctx);
	pthread_create(&thread[1], NULL, consumer, &ctx);
	pthread_join(thread[0], NULL);
	pthread_join(thread[1], NULL);
	return 0;
}
