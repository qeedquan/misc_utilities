#include <cstdio>
#include <iostream>
#include <array>
#include <vector>

using namespace std;

template<typename T>
void print(vector<T> &a)
{
	for (auto &v : a)
		cout << v << " ";
	cout << endl;
}

template<typename T, size_t N>
void print(array<T, N> &a)
{
	for (auto &v : a)
		cout << v << " ";
	cout << endl;
}

void test_array()
{
	vector<array<int, 128> > v1;
	for (int i = 0; i < 128; i++)
	{
		array<int, 128> a = { i, i + 1, i + 2 };
		v1.push_back(a);
	}
	for (auto &a : v1)
		printf("%d %d %d\n", a[0], a[1], a[2]);

	vector<array<char, 80> > v2;
	for (int i = 0; i < 128; i++)
	{
		array<char, 80> s = {};
		snprintf(&s[0], sizeof(s), "test %d size %zu", i, sizeof(s));
		v2.push_back(s);
	}
	for (auto &s : v2)
		printf("%s\n", &s[0]);
	
	array<int, 20> a = {1, 2, 3, 4, 5, 6};
	array<int, 19+1> b = a;
	cout << &a[0] << " " << &b[0] << endl;
	cout << a.size() << " " << b.size() << endl;
	b[3] = 100;
	for (auto v : a)
		cout << v << " ";
	cout << endl;
	for (auto v : b)
		cout << v << " ";
	cout << endl;
}

void test_copy()
{
	vector<int> a = {1, 2, 3, 4, 5, 6, 7};
	vector<int> b = a;
	
	b[0] = 4;
	print(a);
	print(b);
	cout << &a[0] << endl;
	cout << &b[0] << endl;
}

void test_multi_array()
{
	vector<array<int, 10>> a, b;
	array<int, 10> c = {1, 2, 3, 4, 5, 6};
	a.push_back(c);
	a.push_back(c);
	a[0][2] = 20;
	b = a;
	b[0][2] = 30;
	
	printf("\n");
	for (size_t i = 0; i < a.size(); i++)
	{
		print(a[i]);
		print(b[i]);
		cout << &a[i][0] << " " << &b[i][0] << endl;
	}
}

int main()
{
	test_array();
	test_copy();
	test_multi_array();
	return 0;
}
