#include <cstdio>

struct foo
{
	foo()
	{
		printf("foo constructor\n");
	}
	
	void fury()
	{
		printf("fury\n");
	}
};

struct bar
{
	bar()
	{
		printf("bar constructor\n");
	}

	void baseline()
	{
		printf("baseline\n");
	}
};

struct baz
{
	foo f;
	bar b;

	operator foo() { return f; }
	operator bar() { return b; }
};

struct boo
{
	static baz make()
	{
		return baz();
	}
};

int main()
{
	boo::make();
	foo f = boo::make();
	bar b = boo::make();
	f.fury();
	b.baseline();
	return 0;
}
