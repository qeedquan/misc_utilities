// http://www2.math.uu.se/~warwick/main/papers/ECM04Tucker.pdf
// http://cs.emis.de/LNI/Seminar/Seminar07/148.pdf

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

template <typename T>
struct Interval
{
	T a, b;

	Interval operator-() { return Interval{-b, -a}; }

	Interval operator+(const Interval &i) { return Interval{a + i.a, b + i.b}; }
	Interval operator-(const Interval &i) { return Interval{a - i.b, b - i.a}; }

	Interval operator+(const T v) { return Interval{a + v, b + v}; };
	Interval operator-(const T v) { return Interval{a - v, b - v}; };
	Interval operator*(const T v) { return *this * Interval{v, v}; };
	Interval operator/(const T v) { return *this / Interval{v, v}; };

	Interval operator*(const Interval &i)
	{
		auto x = min(a * i.a, min(a * i.b, min(b * i.a, b * i.b)));
		auto y = max(a * i.a, max(a * i.b, max(b * i.a, b * i.b)));
		return Interval{x, y};
	}

	Interval operator/(const Interval &i)
	{
		auto v = Interval{1 / i.b, 1 / i.a};
		return *this * v;
	}

	string str()
	{
		stringstream ss;

		ss << "(" << a << ", " << b << ")";
		return ss.str();
	}
};

template <typename T>
Interval<T> exp(Interval<T> x)
{
	return Interval<T>{exp(x.a), exp(x.b)};
}

template <typename T>
Interval<T> log(Interval<T> x)
{
	return Interval<T>{log(x.a), log(x.b)};
}

template <typename T>
Interval<T> hausdorff_distance(Interval<T> a, Interval<T> b)
{
	return max(abs(a.a - b.a), abs(a.b - b.b));
}

typedef Interval<double> Intervald;

int main()
{
	auto a = Intervald{1, 2};
	auto b = Intervald{5, 7};
	auto c = Intervald{2, 3};
	auto v = a * c + b;
	cout << v.str() << endl;

	a = Intervald{5, 7};
	b = Intervald{1, 2};
	v = -a / b;
	cout << v.str() << endl;

	a = Intervald{-1, 1};
	b = Intervald{-1, 0};
	c = Intervald{3, 4};
	v = a * (b + c);
	cout << v.str() << endl;
	v = a * b + a * c;
	cout << v.str() << endl;

	a = Intervald{0.281, 0.282};
	v = a / 2.0;
	cout << v.str() << endl;

	a = Intervald{-0.532, -0.531};
	b = Intervald{0, 1};
	v = (a * a) / 2 * b;
	cout << v.str() << endl;

	return 0;
}