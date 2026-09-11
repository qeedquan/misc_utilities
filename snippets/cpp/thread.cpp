#include <cstdio>
#include <thread>

using namespace std;

#define print_dot() printf("%s:%d:%s\n", __FILE__, __LINE__, __func__)

struct Foo
{
	void runner()
	{
		print_dot();
	}
};

void hello()
{
	print_dot();
}

void test_basic()
{
	thread t1(hello);
	thread t2(&Foo::runner, Foo());
	auto t3 = new thread(hello);
	t1.join();
	t2.join();
	t3->join();
	delete t3;
}

int main()
{
	test_basic();
	return 0;
}
