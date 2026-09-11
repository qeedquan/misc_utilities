// http://homepage.divms.uiowa.edu/~jones/opsys/notes/27.shtml
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <inttypes.h>
#include <time.h>

typedef struct {
	union {
		max_align_t align;
		void *next;
	};
	char data[];
} fbahdr_t;

typedef struct {
	void *mem;

	fbahdr_t *free;

	size_t nuse;
	size_t nfree;
	size_t blksz;
} fba_t;

uintmax_t
roundup(uintmax_t n, uintmax_t m)
{
	return ((n + m - 1) / m) * m;
}

int
fbainit(fba_t *f, size_t memsz, size_t blksz)
{
	size_t i, nb, sz;
	fbahdr_t *h;
	char *p;

	sz = roundup(blksz + sizeof(fbahdr_t), sizeof(fbahdr_t));
	nb = memsz / blksz;
	p = aligned_alloc(sizeof(fbahdr_t), sz * nb);
	if (!p)
		return -errno;

	memset(f, 0, sizeof(*f));
	for (i = 0; i < nb; i++) {
		h = (fbahdr_t *)(p + i * sz);
		h->next = f->free;
		f->free = h;
	}
	f->mem = p;
	f->blksz = blksz;
	f->nfree = nb;
	return 0;
}

void
fbadestroy(fba_t *f)
{
	free(f->mem);
	memset(f, 0, sizeof(*f));
}

void *
fbaalloc(fba_t *f, size_t sz)
{
	fbahdr_t *h;

	if (sz > f->blksz || f->nfree == 0)
		return NULL;

	f->nuse++;
	f->nfree--;

	h = f->free;
	f->free = h->next;
	return h->data;
}

void
fbafree(fba_t *f, void *p)
{
	fbahdr_t *h;

	if (!p)
		return;

	f->nuse--;
	f->nfree++;

	h = (fbahdr_t *)p - 1;
	h->next = f->free;
	f->free = h;
}

void
t_repalloc(size_t memsz, size_t blksz)
{
	fba_t f[1];
	void *p;
	intmax_t i;
	size_t sz;

	assert(fbainit(f, memsz, blksz) == 0);
	for (i = 1; i <= 1000000; i++) {
		sz = (rand() % blksz) + 1;
		p = fbaalloc(f, sz);
		assert(p);
		fbafree(f, p);
	}
	fbadestroy(f);
}

int
main(void)
{
	srand(time(NULL));

	t_repalloc(1024 * 1024, 1024);
	t_repalloc(1024 * 1024 * 1024, 8192);

	return 0;
}
