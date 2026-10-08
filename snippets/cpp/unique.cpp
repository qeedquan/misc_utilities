#include <iostream>
#include <memory>

using namespace std;

void test_unique_alloc()
{
	unique_ptr<int> val(new int(15));
	for (auto i = 0; i < 100; i++)
	{
		val.reset(new int(i));
		cout << *val << endl;
	}
}

void test_unique_move()
{
	unique_ptr<int> x(new int(200));
	unique_ptr<int> y(move(x));
	cout << boolalpha << static_cast<bool>(x) << endl;
}

int main()
{
	test_unique_alloc();
	test_unique_move();
	return 0;
}
