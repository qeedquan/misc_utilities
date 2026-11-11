#include <iostream>
#include <boost/math/quaternion.hpp>

using namespace std;
using namespace boost::math;

int main()
{
	quaternion q(1.0, 2.0, 3.0, 4.0);
	quaternion s(5.0, 6.0, 7.0, 8.0);

	cout << sin(q) << endl;
	cout << cos(q) << endl;
	cout << tan(q) << endl;
	cout << sinh(q) << endl;
	cout << cosh(q) << endl;
	cout << exp(q) << endl;
	cout << endl;

	cout << q + s << endl;
	cout << q - s << endl;
	cout << q * s << endl;
	cout << q / s << endl;

	return 0;
}
