// LCC technique for block alloc, have an arena array
// where each type of allocation goes in a slot
// then put the block header before the memory alloc
// to keep track of it so we can free it later
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>

#define nelem(x) sizeof(x) / sizeof(x[0])

typedef struct Block Block;
typedef union Header Header;

struct Block {
	Block *next;
};

// used to align the header to max alignment
// so some platforms won't be misaligned when
// reading the block fields
union Header {
	Block b;
	max_align_t a;
};

Header *arena[3];

void *
allocate(size_t n, size_t a)
{
	Header *p;

	p = malloc(sizeof(*p) + n);
	if (p == NULL)
		return NULL;

	assert(a < nelem(arena));

	p->b.next = (void *)arena[a];
	arena[a] = p;
	return NULL;
}

void
deallocate(unsigned a)
{
	Header *p, *q;

	assert(a < nelem(arena));
	for (p = arena[a]; p; p = q) {
		q = (void *)p->b.next;
		free(p);
	}
	arena[a] = NULL;
}

int
main(void)
{
	allocate(1000, 2);
	allocate(1000, 1);
	deallocate(2);
	deallocate(1);
	return 0;
}
