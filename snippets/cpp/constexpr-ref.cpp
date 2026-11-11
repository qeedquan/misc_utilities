#include <iostream>

using namespace std;

template <typename T>
[[nodiscard]] constexpr T &myfunc(T &a, T &b)
{
	return (a < b) ? b : a;
}

int main()
{
	int num1 = 42;
	int num2 = 67;
	myfunc(num1, num2) = 99;
	cout << num1 << " " << num2 << endl;

	return 0;
}
