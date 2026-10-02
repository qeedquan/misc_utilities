#include <iostream>
#include <string>

using namespace std;

class String
{
private:
	int val;

public:
	String()
	{
		val = 0;
	}

	operator int()
	{
		cout << "conversion to int" << endl;
		return val++;
	}

	operator double()
	{
		cout << "conversion to double" << endl;
		return val++;
	}

	operator string()
	{
		cout << "conversion to string" << endl;
		return to_string(val++);
	}
};

void test_string()
{
	String my_string;

	string s1 = my_string;
	string s2 = my_string;
	string s3 = my_string;
	cout << s1 << endl;
	cout << s2 << endl;
	cout << s3 << endl;

	static_cast<string>(my_string);
	string s4 = static_cast<string>(my_string);
	cout << s4 << endl;

	static_cast<int>(my_string);
	static_cast<double>(my_string);
}

int main()
{
	test_string();
	return 0;
}
