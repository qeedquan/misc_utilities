// http://www.mathcs.emory.edu/~cheung/Courses/323/Syllabus/Map/skip-list-impl.html
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

typedef struct skiplist_t skiplist_t;
typedef struct skipnode_t skipnode_t;

struct skipnode_t {
	long key;
	int val;
	skipnode_t *up;
	skipnode_t *down;
	skipnode_t *left;
	skipnode_t *right;
};

struct skiplist_t {
	skipnode_t *head;
	skipnode_t *tail;
	size_t len;
	size_t height;
};

long
djbhash(const char *str)
{
	long hash = 5381;
	int c;

	while ((c = *str++))
		hash = ((hash << 5) + hash) + c;

	return hash;
}

void *
xcalloc(size_t nmemb, size_t size)
{
	void *p;

	if (nmemb == 0)
		nmemb = 1;
	if (size == 0)
		size = 1;
	p = calloc(nmemb, size);
	if (p == NULL)
		abort();
	return p;
}

void
skiplist_free(skiplist_t *l)
{
	skipnode_t *h, *d, *n, *nn;

	if (l == NULL)
		return;

	h = l->head;
	for (;;) {
		d = NULL;
		for (n = h; n != NULL; n = nn) {
			if (d == NULL)
				d = n->down;
			nn = n->right;
			free(n);
		}
		if (d == NULL)
			break;
		h = d;
	}
	free(l);
}

skipnode_t *
skiplist_new_node(const char *key, int val)
{
	skipnode_t *n;

	n = xcalloc(1, sizeof(*n));
	if (n == NULL)
		return NULL;

	if (!strcmp(key, "-oo"))
		n->key = LONG_MIN;
	else if (!strcmp(key, "oo"))
		n->key = LONG_MAX;
	else
		n->key = djbhash(key);

	n->val = val;
	return n;
}

skiplist_t *
skiplist_new(void)
{
	skiplist_t *l;
	skipnode_t *n1, *n2;

	l = xcalloc(1, sizeof(*l));
	if (l == NULL)
		goto error;

	n1 = skiplist_new_node("-oo", 0);
	n2 = skiplist_new_node("oo", 0);
	if (n1 == NULL || n2 == NULL)
		goto error;

	n1->right = n2;
	n2->left = n1;

	l->head = n1;
	l->tail = n2;

	return l;

error:
	skiplist_free(l);
	return NULL;
}

skipnode_t *
skiplist_find(skiplist_t *l, const char *key)
{
	skipnode_t *p;
	long k;

	k = djbhash(key);
	p = l->head;
	for (;;) {
		while (p->right->key != LONG_MAX && p->right->key <= k)
			p = p->right;

		if (p->down != NULL)
			p = p->down;
		else
			break;
	}

	return p;
}

bool
skiplist_get(skiplist_t *l, const char *key, int *val)
{
	skipnode_t *n;

	n = skiplist_find(l, key);
	if (n->key == djbhash(key)) {
		if (val)
			*val = n->val;
		return true;
	}
	return false;
}

bool
skiplist_add_layer(skiplist_t *l)
{
	skipnode_t *p1, *p2;

	p1 = skiplist_new_node("-oo", 0);
	p2 = skiplist_new_node("oo", 0);
	if (p1 == NULL || p2 == NULL)
		goto error;

	p1->right = p2;
	p1->down = l->head;

	p2->left = p1;
	p2->down = l->tail;

	l->head->up = p1;
	l->tail->up = p2;

	l->head = p1;
	l->tail = p2;

	l->height++;

	return true;

error:
	free(p1);
	free(p2);
	return false;
}

bool
skiplist_put(skiplist_t *l, const char *key, int val)
{
	skipnode_t *e, *p, *q;
	size_t i;

	p = skiplist_find(l, key);
	if (p->key == djbhash(key)) {
		p->val = val;
		return true;
	}
	q = skiplist_new_node(key, val);
	if (q == NULL)
		return false;
	q->left = p;
	q->right = p->right;
	p->right->left = q;
	p->right = q;

	for (i = 0; drand48() < 0.5; i++) {
		if (i >= l->height) {
			if (!skiplist_add_layer(l))
				break;
		}

		while (p->up == NULL)
			p = p->left;
		p = p->up;

		e = skiplist_new_node(key, 0);
		if (e == NULL)
			break;

		e->left = p;
		e->right = p->right;
		e->down = q;

		p->right->left = e;
		p->right = e;
		q->up = e;

		q = e;
	}

	l->len++;
	return true;
}

void
skiplist_print(skiplist_t *l)
{
	skipnode_t *h, *d, *n;

	h = l->head;
	for (;;) {
		d = NULL;
		for (n = h; n != NULL; n = n->right) {
			printf("(%ld, %d) ", n->key, n->val);
			if (d == NULL)
				d = n->down;
		}
		if (d == NULL)
			break;
		h = d;
		printf("\n");
	}
}

int
main(void)
{
	skiplist_t *l;
	char buf[20];
	int i;

	l = skiplist_new();
	for (i = 0; i < 32; i++) {
		snprintf(buf, sizeof(buf), "%d", i);
		skiplist_put(l, buf, i);
		skiplist_print(l);
		printf("\n======\n");
	}
	skiplist_free(l);
	return 0;
}
