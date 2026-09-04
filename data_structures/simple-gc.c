// based on allocator from http://piumarta.com/software/lysp/

#define _GNU_SOURCE
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

typedef struct gc_stack_root_t gc_stack_root_t;
typedef struct gc_stats_t gc_stats_t;
typedef struct gc_header_t gc_header_t;
typedef struct gc_t gc_t;

typedef void (*gc_mark_function_t)(gc_t *, void *);
typedef void (*gc_free_function_t)(gc_t *, void *);

struct gc_stack_root_t {
	void **root;
	gc_stack_root_t *next;
};

struct gc_stats_t {
	size_t objects;
	size_t bytes;
};

struct gc_header_t {
	union {
		max_align_t align;
		struct {
			union {
				unsigned flags;
				struct {
					unsigned used : 1;
					unsigned atom : 1;
					unsigned mark : 1;
				};
			};
			gc_header_t *prev;
			gc_header_t *next;
			size_t size;
		};
	};
};

#define gc_protect(gc, v)                       \
	gc_stack_root_t _sr_##v = {.root = &v}; \
	gc_push_root(gc, &_sr_##v)

#define gc_unprotect(gc, v) gc_pop_root(gc, &_sr_##v)

struct gc_t {
	// sentinel to mark the end of the objects
	gc_header_t base;
	// the objects allocated managed by the gc
	gc_header_t *objects;

	// allocation count, will trigger a gc cycle
	// when it exceeds allocs per gc cycle
	unsigned count;
	// number of allocs before a gc cycle
	unsigned allocs_per_gc;

	gc_stack_root_t *stack_roots;
	void ***roots;
	size_t num_roots;
	size_t max_roots;

	gc_mark_function_t mark;
	gc_free_function_t free;

	unsigned quantum;
	int verbose;
};

static void *
hdr2ptr(gc_header_t *hdr)
{
	return hdr + 1;
}

static gc_header_t *
ptr2hdr(void *ptr)
{
	return (gc_header_t *)ptr - 1;
}

static void
gc_push_root(gc_t *gc, gc_stack_root_t *sr)
{
	sr->next = gc->stack_roots;
	gc->stack_roots = sr;
}

static void
gc_pop_root(gc_t *gc, gc_stack_root_t *sr)
{
	gc->stack_roots = sr->next;
}

static gc_header_t *
gc_free_header(gc_header_t *hdr)
{
	// this header is now marked free for reuse
	hdr->flags = 0;

	// do coalescing to get more memory on prev and next
	// pointers
	if (!hdr->prev->flags && (char *)hdr2ptr(hdr->prev) + hdr->prev->size == (char *)hdr) {
		hdr->prev->next = hdr->next;
		hdr->next->prev = hdr->prev;
		hdr->prev->size += sizeof(gc_header_t) + hdr->size;
		hdr = hdr->prev;
	}
	if (!hdr->next->used && (char *)hdr2ptr(hdr) + hdr->size == (char *)hdr->next) {
		hdr->size += sizeof(gc_header_t) + hdr->next->size;
		hdr->next = hdr->next->next;
		hdr->next->prev = hdr;
	}
	return hdr;
}

static void gc_mark(gc_t *, void *);

// mark sub-objects in the pointer
static void
gc_mark_sub(gc_t *gc, void *ptr)
{
	gc_header_t *hdr;
	void **pos, **lim, *field;

	// loop through the memory blocks searching for
	// pointers so we can mark it
	hdr = ptr2hdr(ptr);
	pos = ptr;
	lim = (void **)((char *)hdr2ptr(hdr) + hdr->size - sizeof(void *));
	for (; pos <= lim; pos++) {
		field = *pos;

		// pointer are always aligned so this
		// check figures out if we are a pointer
		// or some other type, if it is a pointer
		// we will recursively traverse that object
		// to mark its sub-objects
		if (field && !((uintptr_t)(field)&1))
			gc_mark(gc, field);
	}
}

// mark an individual object
// so we can free it later
static void
gc_mark(gc_t *gc, void *ptr)
{
	gc_header_t *hdr;

	hdr = ptr2hdr(ptr);
	if (!hdr->mark) {
		hdr->mark = 1;
		// if the memory allocated was not atomic
		// we have to go through it's sub-objects
		// and mark those too
		if (!hdr->atom)
			gc->mark(gc, ptr);
	}
}

// goes through the objects and frees them
void
gc_sweep(gc_t *gc)
{
	gc_header_t *hdr;

	hdr = gc->base.next;
	do {
		// if it is not marked, we don't free it
		if (hdr->flags) {
			if (hdr->mark)
				hdr->mark = 0;
			else {
				if (gc->free)
					gc->free(gc, hdr2ptr(hdr));
				hdr = gc_free_header(hdr);
			}
		}
		hdr = hdr->next;
	} while (hdr != &gc->base);

	gc->objects = gc->base.next;
}

// runs a gc collection
// freeing up any unused memory
static void
gc_gcollect(gc_t *gc)
{
	gc_stack_root_t *sr;
	size_t i;

	// collect the custom roots we added
	for (i = 0; i < gc->num_roots; i++) {
		if (*gc->roots[i])
			gc_mark(gc, *gc->roots[i]);
	}

	for (sr = gc->stack_roots; sr; sr = sr->next) {
		if (*(sr->root))
			gc_mark(gc, *(sr->root));
	}
	gc_sweep(gc);
}

// add objects with subojects for the gc to manage
int
gc_add_root(gc_t *gc, void *root)
{
	size_t size;
	void ***roots;

	if (gc->num_roots == gc->max_roots) {
		roots = gc->roots;
		size = (gc->max_roots == 0) ? 128 : gc->max_roots * 2;
		roots = realloc(roots, sizeof(roots[0]) * size);
		if (!roots)
			return -1;
	}
	gc->roots[gc->num_roots++] = (void **)root;
	return 0;
}

void
gc_delete_root(gc_t *gc, void *root)
{
	size_t i;

	for (i = 0; i < gc->num_roots; i++) {
		if (gc->roots[i] == root)
			break;
	}
	if (i < gc->num_roots) {
		memmove(gc->roots + i, gc->roots + i + 1, sizeof(gc->roots[0]) * (gc->num_roots - i));
		gc->num_roots--;
	}
}

void *
gc_malloc(gc_t *gc, size_t size)
{
	static const size_t align = sizeof(max_align_t);

	struct gc_header_t *hdr, *org, *ins;
	size_t incr, req;
	void *mem;

	// we do a mark and sweep every allocs_per_gc cycle
	// to reclaim unused objects
	if (gc->count++ >= gc->allocs_per_gc) {
		gc->count = 0;
		gc_gcollect(gc);
	}

	// align allocation size
	size = (size + align - 1) & ~(align - 1);

	// if we have enough memory just allocate from the pool rather than asking
	// for more memory
	hdr = org = gc->objects;
	do {
		if (!hdr->used && hdr->size >= size) {
			// if the header region can handle this allocate
			// we just use the memory inside the region
			// otherwise we use the whole region for the allocation
			if (hdr->size >= size + sizeof(gc_header_t) + align) {
				ins = (gc_header_t *)((char *)hdr2ptr(hdr) + size);
				ins->flags = 0;
				ins->prev = hdr;
				ins->next = hdr->next;
				ins->size = hdr->size - size - sizeof(gc_header_t);
				hdr->next->prev = ins;
				hdr->next = ins;
				hdr->size = size;
			}
			hdr->used = 1;
			gc->objects = hdr->next;
			mem = hdr2ptr(hdr);
			memset(mem, 0, hdr->size);
			return mem;
		}
		hdr = hdr->next;
	} while (hdr != org);

	// starting size is 'quantum' bytes
	// adjust if needed to be able to fit
	// request allocation
	incr = gc->quantum;
	req = sizeof(gc_header_t) + size;
	while (incr <= req)
		incr *= 2;
	hdr = sbrk(incr);
	if (hdr == (gc_header_t *)-1)
		return NULL;
	hdr->flags = 0;
	hdr->next = &gc->base;
	hdr->prev = gc->base.prev;
	hdr->prev->next = hdr;
	gc->base.prev = hdr;
	hdr->size = incr - sizeof(gc_header_t);

	mem = hdr2ptr(hdr);
	memset(mem, 0, hdr->size);
	return mem;
}

// atomic memory do not get marked and freed
void *
gc_malloc_atomic(gc_t *gc, size_t size)
{
	void *mem;

	mem = gc_malloc(gc, size);
	if (!mem)
		return NULL;
	ptr2hdr(mem)->atom = 1;
	return mem;
}

void
gc_free(gc_t *gc, void *ptr)
{
	if (ptr == NULL)
		return;
	gc->objects = gc_free_header(ptr2hdr(ptr));
}

void *
gc_realloc(gc_t *gc, void *ptr, size_t size)
{
	gc_header_t *hdr;
	void *mem;

	hdr = ptr2hdr(ptr);
	if (size <= hdr->size)
		return ptr;

	mem = gc_malloc(gc, size);
	if (!mem)
		return NULL;

	memcpy(mem, ptr, hdr->size);
	ptr2hdr(mem)->atom = hdr->atom;
	gc_free(gc, ptr);
	return mem;
}

void
gc_collect_stats(gc_t *gc, gc_stats_t *st)
{
	gc_header_t *hdr;

	memset(st, 0, sizeof(*st));
	hdr = gc->base.next;
	do {
		if (hdr->flags) {
			st->objects++;
			st->bytes += hdr->size;
		}
		hdr = hdr->next;
	} while (hdr != &gc->base);
}

gc_t defgc = {
    .base = {
        .flags = ~0u,
        .prev = &defgc.base,
        .next = &defgc.base,
        .size = 0,
    },
    .objects = &defgc.base,
    .allocs_per_gc = 10000,
    .quantum = 32768,
    .verbose = 0,
    .mark = gc_mark_sub,
};

#define gprotect(v) gc_protect(&defgc, v)
#define gunprotect(v) gc_unprotect(&defgc, v)

void *
gmalloc(size_t size)
{
	return gc_malloc(&defgc, size);
}

void *
grealloc(void *ptr, size_t size)
{
	return gc_realloc(&defgc, ptr, size);
}

void
gfree(void *ptr)
{
	gc_free(&defgc, ptr);
}

void
gsweep(void)
{
	gc_sweep(&defgc);
}

void
gstats(void)
{
	gc_stats_t st;

	gc_collect_stats(&defgc, &st);
	printf("objects %zu bytes %zu\n", st.objects, st.bytes);
}

int
main(void)
{
	for (int i = 0; i < 10000; i++) {
		char *p = gmalloc(2000);
		gmalloc(1000);
		strcpy(p, "qwertyuiopz");
	}
	gsweep();

	void *a = gmalloc(1000);
	gprotect(a);
	gunprotect(a);
	return 0;
}
