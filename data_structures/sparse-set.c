// https://research.swtch.com/sparse
// clever use of 2 arrays to add/check members without initializing
// the data structure
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
	size_t n;
	size_t l;
	size_t *p;
	size_t d[];
} Set;

Set *
setnew(size_t size)
{
	Set *s;

	s = malloc(sizeof(*s) + 2 * size * sizeof(*s->d));
	if (!s)
		return NULL;
	s->n = 0;
	s->p = s->d + size;
	s->l = size;
	return s;
}

void
setadd(Set *s, size_t i)
{
	assert(i < s->l);
	s->d[s->n] = i;
	s->p[i] = s->n;
	s->n++;
}

int
sethas(Set *s, size_t i)
{
	assert(i < s->l);
	return s->p[i] < s->n && s->d[s->p[i]] == i;
}

void
setclr(Set *s)
{
	s->n = 0;
}

void
setdump(Set *s)
{
	size_t i;

	for (i = 0; i < s->n; i++)
		printf("%zu ", s->d[i]);
	printf("\n");
}

int
main(void)
{
	Set *s;

	assert(s = setnew(1024));
	setadd(s, 512);
	assert(sethas(s, 100) == 0);
	assert(sethas(s, 512) == 1);
	setadd(s, 100);
	assert(sethas(s, 100) == 1);
	setadd(s, 452);
	setadd(s, 1);
	setdump(s);
	free(s);
	return 0;
}
