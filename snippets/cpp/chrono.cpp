#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

void test_clock_source()
{
	for (int i = 0; i < 4; i++)
	{
		auto s0 = chrono::steady_clock::now();
		auto s1 = chrono::system_clock::now();
		auto s2 = chrono::high_resolution_clock::now();

		this_thread::sleep_for(std::chrono::milliseconds(1000));

		auto e0 = chrono::steady_clock::now();
		auto e1 = chrono::system_clock::now();
		auto e2 = chrono::high_resolution_clock::now();

		cout << chrono::duration_cast<chrono::nanoseconds>(e0 - s0).count() << " ns" << endl;
		cout << chrono::duration_cast<chrono::nanoseconds>(e1 - s1).count() << " ns" << endl;
		cout << chrono::duration_cast<chrono::nanoseconds>(e2 - s2).count() << " ns" << endl;
		cout << endl;

		cout << chrono::duration_cast<chrono::microseconds>(e0 - s0).count() << " us" << endl;
		cout << chrono::duration_cast<chrono::microseconds>(e1 - s1).count() << " us" << endl;
		cout << chrono::duration_cast<chrono::microseconds>(e2 - s2).count() << " us" << endl;
		cout << endl;

		cout << chrono::duration_cast<chrono::milliseconds>(e0 - s0).count() << " ms" << endl;
		cout << chrono::duration_cast<chrono::milliseconds>(e1 - s1).count() << " ms" << endl;
		cout << chrono::duration_cast<chrono::milliseconds>(e2 - s2).count() << " ms" << endl;
		cout << endl;

		cout << chrono::duration_cast<chrono::seconds>(e0 - s0).count() << " s" << endl;
		cout << chrono::duration_cast<chrono::seconds>(e1 - s1).count() << " s" << endl;
		cout << chrono::duration_cast<chrono::seconds>(e2 - s2).count() << " s" << endl;
		cout << endl;
	}
}

void test_conversion()
{
	auto t0 = chrono::milliseconds(0);
	auto dt = chrono::milliseconds(1);
	for (auto i = 0; i < 10; i++)
	{
		t0 += dt;
		cout << t0.count() << endl;
	}
	for (auto i = 0; i < 10; i++)
	{
		t0 += chrono::milliseconds((int)1.3);
		cout << t0.count() << endl;
	}
}

int main()
{
	test_conversion();
	test_clock_source();
	return 0;
}
