#include <cstdio>

struct NoCopy
{
	NoCopy()
	{
		printf("%s\n", __func__);
	}

	~NoCopy()
	{
		printf("%s\n", __func__);
	}

	// disallows copying
	NoCopy(NoCopy const &other) = delete;
	NoCopy& operator=(NoCopy const &other) = delete;
};

struct Foo
{
	NoCopy x;
};

struct Bar : NoCopy
{
	NoCopy *y;
};

int main()
{
	NoCopy a, b, *c = new NoCopy();
	Foo f, g;
	Bar *x = new Bar(), *y;

	// compile error
	// a = b;
	// a = *c;
	// g = f;
	
	// allowed if we indirect
	y = x;

	delete c;
	delete y;

	return 0;
}
