#include <iostream>
#include <string>

using namespace std;

struct Foo
{
	Foo(const string &name, int value) : name(name), value(value)
	{
	}

	string name;
	int value;
};

struct Bar
{
	int x;
};

// can add more printing options to cout by defining the following operator overloads
ostream &operator<<(ostream &out, Foo &f)
{
	out << "foo(" << f.name << ", " << f.value << ")";
	return out;
}

ostream &operator<<(ostream &out, Bar &b)
{
	out << "bar(" << b.x << ")";
	return out;
}

int main()
{
	Foo a("abc", 10);
	Bar b = Bar{ 1 };

	cout << a << endl
	     << b << endl;
	return 0;
}
