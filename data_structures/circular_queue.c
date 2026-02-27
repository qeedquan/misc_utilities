// https://en.wikipedia.org/wiki/Circular_buffer
// https://www.snellman.net/blog/archive/2016-12-13-ring-buffers/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef struct {
	int *data;
	size_t rp, wp;
	size_t len, cap;
} CQ;

int
mkcq(CQ *cq, size_t cap)
{
	cq->data = calloc(sizeof(*cq->data), cap);
	if (cq->data == NULL)
		return -1;
	cq->cap = cap;
	cq->len = 0;
	cq->rp = 0;
	cq->wp = 0;
	return 0;
}

void
cqfree(CQ *cq)
{
	if (cq == NULL)
		return;
	free(cq->data);
}

int
cqenq(CQ *cq, int val)
{
	if (cq->len >= cq->cap)
		return -1;
	cq->data[cq->wp] = val;
	cq->wp = (cq->wp + 1) % cq->cap;
	cq->len++;
	return 0;
}

int
cqdeq(CQ *cq, int *val)
{
	*val = 0;
	if (cq->len == 0)
		return -1;
	*val = cq->data[cq->rp];
	cq->rp = (cq->rp + 1) % cq->cap;
	cq->len--;
	return 0;
}

int
cqpeek(CQ *cq)
{
	if (cq->len == 0)
		return INT_MIN;
	return cq->data[cq->rp];
}

void
cqdump(CQ *cq, size_t len)
{
	size_t i;

	for (i = 0; i < len; i++)
		printf("%d ", cq->data[(cq->rp + i) % cq->cap]);
	printf("\n");
}

void
lnvenq(CQ *cq, int val)
{
	if (cq->len >= cq->cap) {
		cq->rp = (cq->rp + 1) % cq->cap;
		cq->len--;
	}
	cq->data[cq->wp] = val;
	cq->wp = (cq->wp + 1) % cq->cap;
	cq->len++;
}

int
mkcqp2(CQ *cq, size_t cap)
{
	assert((cap & (cap - 1)) == 0);
	return mkcq(cq, cap);
}

size_t
cqp2len(CQ *cq)
{
	return cq->wp - cq->rp;
}

int
cqp2enq(CQ *cq, int val)
{
	if (cqp2len(cq) >= cq->cap)
		return -1;
	cq->data[cq->wp++ & (cq->cap - 1)] = val;
	return 0;
}

int
cqp2deq(CQ *cq, int *val)
{
	*val = 0;
	if (cqp2len(cq) == 0)
		return -1;
	*val = cq->data[cq->rp++ & (cq->cap - 1)];
	return 0;
}

void
lnvp2enq(CQ *cq, int val)
{
	if (cqp2len(cq) >= cq->cap)
		cq->rp++;
	cq->data[cq->wp++ & (cq->cap - 1)] = val;
}

// bounded circular queues enqueues up to cap elements
// and dequeues as a regular queue would, but with
// O(1) behavior since we don't need to move the
// array whenever enqueue/dequeue occurs
void
testcq(void)
{
	CQ cq[1];
	int i, val;

	printf("Bounded Circular Queues\n");
	mkcq(cq, 5);
	cqenq(cq, 20);
	cqenq(cq, 30);
	cqenq(cq, 40);

	cqdeq(cq, &val);
	printf("%d\n", val);

	cqdeq(cq, &val);
	printf("%d\n", val);

	for (i = 0; i < 100; i++)
		cqenq(cq, i);
	for (i = 0; i < 100; i++) {
		if (cqdeq(cq, &val) < 0)
			break;
		printf("%d %d\n", i, val);
	}
	printf("%zu\n", cq->len);
	cqenq(cq, 62);
	printf("%zu %d\n", cq->len, cqpeek(cq));
	cqfree(cq);

	printf("\n\n");
}

// last nth value circular queue stores the last nth value
// and overwrites oldest entries, useful for rolling statistics
void
testlnv(void)
{
	CQ cq[1];
	int i;

	mkcq(cq, 8);
	printf("Last Nth Value\n");
	for (i = 0; i < 100; i++) {
		lnvenq(cq, i);
		cqdump(cq, cq->len);
	}
	printf("\n\n");
	cqfree(cq);
}

// this should behave the same as a bounded circular queue
// we just restrict the capacity to be power of 2 to make it faster
// to enqueue/dequeue and use less memory since we don't store
// the length anymore
void
testcqp2(void)
{
	CQ cq[1];
	size_t i, N;
	int val;

	N = 4;
	mkcqp2(cq, N);
	printf("Bounded Circular Queues/Last Nth Value - Power of Two\n");
	for (i = 0; i < N * 2; i++)
		cqp2enq(cq, i * 10);
	for (i = 0; i < N / 2; i++)
		cqp2deq(cq, &val);
	cqdump(cq, cqp2len(cq));

	for (i = 0; i < 2 * N; i++)
		cqp2enq(cq, i);
	cqdump(cq, cqp2len(cq));
	for (i = 0; i < N - 1; i++)
		cqp2deq(cq, &val);
	cqdump(cq, cqp2len(cq));
	cqp2deq(cq, &val);

	for (i = 0; i < 100; i++) {
		lnvp2enq(cq, i);
		cqdump(cq, cqp2len(cq));
	}

	cqfree(cq);
}

int
main(void)
{
	testcq();
	testlnv();
	testcqp2();
	return 0;
}
