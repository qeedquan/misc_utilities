#include <functional>
#include <cstdio>

using std::function;

class Func
{
public:
	void operator()()
	{
		printf("%s:%d %d %s\n", __func__, __LINE__, val, str);
		val += 10;
	}

private:
	int val = 100;
	const char *str = "func";
};

void fptr()
{
	printf("%s:%d\n", __func__, __LINE__);
}

template <typename T>
void call(function<T> f)
{
	printf("%s:%d\n", __func__, __LINE__);
	f();
	f();
}

int main()
{
	function<void()> f = fptr;
	f();

	auto i = 0;
	auto g = [&i]() { printf("%s:%d %d\n", __func__, __LINE__, i++); };
	f = g;
	for (auto n = 0; n < 32; n++)
	{
		g();
		f();
	}
	call(f);
	f = fptr;
	call(f);
	
	Func fn;
	f = fn;
	f();
	f();
	f();
	fn();

	return 0;
}
