#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))
#define min(a, b) (((a) < (b)) ? (a) : (b))

typedef struct {
	pthread_t tid;
	int dfd;
	int val;
} sldata_t;

static void *
slworker(void *arg)
{
	sldata_t *s;

	s = arg;
	sleep(s->val);
	write(s->dfd, &s->val, sizeof(s->val));
	return NULL;
}

int
slsort(int *data, size_t size)
{
	sldata_t *d, *s;
	int p[2], v, rv, b;
	size_t i, j;

	rv = 0;
	i = 0;
	d = NULL;
	p[0] = -1;
	p[1] = -1;

	if (size == 0)
		return 0;

	if (pipe(p) < 0)
		goto error;

	d = calloc(size, sizeof(*d));
	if (d == NULL)
		goto error;

	b = data[0];
	for (i = 1; i < size; i++)
		b = min(b, data[i]);

	for (i = 0; i < size; i++) {
		s = d + i;
		s->dfd = p[1];
		s->val = data[i] - b;

		if (pthread_create(&s->tid, NULL, slworker, s) != 0)
			goto error;
	}

	if (0) {
	error:
		rv = -1;
	}
	for (j = 0; j < i; j++) {
		read(p[0], &v, sizeof(v));
		printf("%d ", v + b);
	}
	if (j != 0)
		printf("\n");

	for (j = 0; j < i; j++)
		pthread_join(d[j].tid, NULL);

	free(d);
	if (p[0] >= 0)
		close(p[0]);
	if (p[1] >= 0)
		close(p[1]);

	return rv;
}

void *
ecalloc(size_t nmemb, size_t size)
{
	void *p;

	p = calloc(nmemb, size);
	if (p == NULL)
		abort();
	return p;
}

int
main(int argc, char *argv[])
{
	int *arr, i;
	size_t len;

	if (argc < 2) {
		fprintf(stderr, "usage: num ...\n");
		exit(1);
	}

	arr = ecalloc(argc - 1, sizeof(*arr));
	len = 0;
	for (i = 1; i < argc; i++) {
		arr[i - 1] = atoi(argv[i]);
		len++;
	}

	setvbuf(stdout, NULL, _IONBF, 0);
	if (slsort(arr, len) < 0)
		fprintf(stderr, "failed to allocate memory for sleepsort\n");
	free(arr);
	return 0;
}
