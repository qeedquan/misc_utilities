#include <iostream>
#include <variant>

using namespace std;

template <typename T, size_t N>
struct array
{
	T values[N];
};

int main()
{
	variant<int, float, double, long, array<char, 30>, short> a;
	cout << sizeof(a) << endl;
	a = 102;
	try
	{
		get<double>(a);
	}
	catch (const bad_variant_access &)
	{
		cout << "not a double" << endl;
	}

	auto x = get<int>(a);
	cout << x << endl;

	cout << holds_alternative<float>(a) << endl;
	cout << holds_alternative<int>(a) << endl;
	cout << holds_alternative<long>(a) << endl;
	cout << holds_alternative<short>(a) << endl;

	return 0;
}
