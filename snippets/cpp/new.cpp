#include <iostream>
#include <cstring>
#include <vector>

using namespace std;

template <typename T>
struct foo
{
	T a[256];
};

struct bar
{
	int x, y, z;

	bar()
	{
		x = 2;
	}

	bar(int x)
	{
		memset(this, 0, sizeof(*this));
		this->x = x;
	}
};

struct baz
{
	vector<int> a;

	baz()
	{
	}
};

void test_basic()
{
	// dont zero init
	auto f = new foo<int>;

	// zero init
	auto g = new foo<int>();

	for (size_t i = 0; i < 256; i++)
		cout << f->a[i] << " " << g->a[i] << endl;

	delete f;
	delete g;
}

void test_constructor()
{
	// does not zero init if constructor is there
	auto a = new bar();

	// use memset in constructor to zero init
	auto b = new bar(2);

	// cannot use memset if the struct has non-trivial constructors inside
	auto c = new baz();

	printf("%d %d %d\n", a->x, a->y, a->z);	
	printf("%d %d %d\n", b->x, b->y, b->z);	
	printf("%zu\n", c->a.size());
	
	delete a;
	delete b;
	delete c;
}

int main()
{
	test_basic();
	test_constructor();
	return 0;
}
