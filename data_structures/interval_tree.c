// https://www.geeksforgeeks.org/interval-tree/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

typedef struct Interval Interval;
typedef struct Node Node;

struct Interval {
	int lo, hi;
};

struct Node {
	Interval i;
	int max;
	Node *left, *right;
};

Node *
nod(Interval i)
{
	Node *n;

	n = calloc(1, sizeof(*n));
	n->i = i;
	n->max = i.hi;
	return n;
}

Node *
insert(Node *root, Interval i)
{
	int lo;

	if (root == NULL)
		return nod(i);

	// get low value of root
	lo = root->i.lo;

	// if root low value is smaller, then
	// new interval go into left subtree
	// otherwise insert i to the right
	if (i.lo < lo)
		root->left = insert(root->left, i);
	else
		root->right = insert(root->right, i);

	// update max value of ancestor if needed
	if (root->max < i.hi)
		root->max = i.hi;

	return root;
}

bool
overlap(Interval i1, Interval i2)
{
	return i1.lo <= i2.hi && i2.lo <= i1.hi;
}

Interval *
searchoverlap(Node *root, Interval i)
{
	if (root == NULL)
		return NULL;

	if (overlap(root->i, i))
		return &root->i;

	// if left child is present and max of left child
	// is greater than given interval, then i may overlap
	// with an interval in left tree
	if (root->left != NULL && root->left->max >= i.lo)
		return searchoverlap(root->left, i);

	// otherwise try right subtree
	return searchoverlap(root->right, i);
}

void
inorder(Node *root)
{
	if (root == NULL)
		return;

	inorder(root->left);
	printf("[%d,%d] max = %d\n", root->i.lo, root->i.hi, root->max);
	inorder(root->right);
}

void
freenod(Node *root)
{
	if (root == NULL)
		return;

	freenod(root->left);
	freenod(root->right);
	free(root);
}

int
main(void)
{
	Interval in[] = {{15, 20}, {10, 30}, {17, 19}, {5, 20}, {12, 15}, {30, 40}};
	Node *root;
	size_t i;

	root = NULL;
	for (i = 0; i < nelem(in); i++)
		root = insert(root, in[i]);

	printf("inorder traversal\n");
	inorder(root);
	freenod(root);
	return 0;
}
