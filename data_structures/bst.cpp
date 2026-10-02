#include <cstdio>
#include <cstdlib>
#include <climits>
#include <queue>

using std::queue;

struct Node
{
	int   data;
	Node *left;
	Node *right;

	Node(int data);
	~Node();
	Node *insert(int data);
	size_t min_depth();
	bool is_bst();

	static void free(Node *n);

private:
	static bool is_bst_rec(Node *n, int min, int max);
};

void print_tree(Node *n)
{
	if (n == nullptr)
		return;

	printf("%d\n", n->data);
	print_tree(n->left);
	print_tree(n->right);
}

int main()
{
	auto n = new Node(1);
	n->insert(0);
	n->insert(2);
	n->insert(3);
	n->insert(4);
	n->insert(5);
	n->insert(6);
	n->insert(7);
	n->insert(8);
	n->insert(9);
	n->insert(-2);
	n->insert(-3);
	n->insert(-4);
	n->insert(-5);
	n->insert(-6);
	print_tree(n);
	printf("depth: %zu\n", n->min_depth());
	printf("is_bst: %d\n", n->is_bst());
	Node::free(n);
	return 0;
}

Node::Node(int data) : data(data), left(nullptr), right(nullptr)
{
}

Node::~Node()
{
}

void Node::free(Node *n)
{
	if (n == nullptr)
		return;
	Node::free(n->left);
	Node::free(n->right);
	delete n;
}

Node *Node::insert(int v)
{
	auto p = new Node(v);
	auto n = this;
	for (;;)
	{
		if (v < data)
		{
			if (n->left == nullptr)
			{
				n->left = p;
				break;
			}
			else
				n = n->left;
		}
		else
		{
			if (n->right == nullptr)
			{
				n->right = p;
				break;
			}
			else
				n = n->right;
		}
	}
	return p;
}

size_t Node::min_depth()
{
	struct Depth
	{
		Node  *node;
		size_t depth;
	};
	queue<Depth> q;
	q.push(Depth{this, 1});

	while (!q.empty())
	{
		auto p = q.front();
		q.pop();

		auto n = p.node;
		if (n->left == nullptr && n->right == nullptr)
			return p.depth;

		if (n->left != nullptr)
			q.push(Depth{n->left, p.depth+1});
		if (n->right != nullptr)
			q.push(Depth{n->right, p.depth+1});
	}
	return 0;
}

bool Node::is_bst()
{
	return Node::is_bst_rec(this, INT_MIN, INT_MAX);
}

bool Node::is_bst_rec(Node *n, int min, int max)
{
	if (n == nullptr)
		return true;

	if (n->data < min || n->data > max)
		return false;

	return is_bst_rec(n->left, min, n->data-1) && is_bst_rec(n->right, n->data+1, max);
}
