#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main()
{
	string str = "C++ string";
	vector<int> vec = { 1, 2, 3, 4, 5 };
	cout << str << endl;
	for (auto val : vec)
		cout << val << endl;
	return 0;
}
