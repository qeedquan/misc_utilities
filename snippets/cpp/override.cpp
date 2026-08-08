#include <cstdio>

struct A
{
	A()
	{
		printf("A count %d\n", count());
	}

	virtual ~A()
	{
	};

	virtual int count()
	{
		return 1;
	}
};

struct B : A
{
	B()
	{
		printf("B count %d\n", count());
	}

	~B()
	{
	}

	int count() override
	{
		return 5;
	}
};

int main()
{
	A a;
	printf("\n");

	B b;
	printf("\n");
	
	A *c = new B;
	printf("C count %d\n", c->count());
	delete c;

	return 0;
}
