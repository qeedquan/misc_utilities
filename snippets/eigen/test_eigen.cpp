#include <iostream>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

int main()
{
	Matrix3f A;
	A << 1, 2, 1,
	    2, 1, 0,
	    -1, 1, 2;
	cout << A << endl;
	cout << A * A * A * A * A * A * A << endl;
	cout << A.inverse() << endl;
	cout << A.determinant() << endl;

	MatrixXf B = MatrixXf::Random(5, 3);
	cout << B << endl;

	return 0;
}
