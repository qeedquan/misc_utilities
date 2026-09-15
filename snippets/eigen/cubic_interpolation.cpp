#include <iostream>
#include <vector>
#include <Eigen/Dense>

int main()
{
	// Sample data points (x and y)
	std::vector<double> x_data = { -1.0, -0.5, 0.0, 0.5, 1.0, 1.5 };
	std::vector<double> y_data = { 1.0, 0.2, 0.0, 0.3, 1.2, 4.0 };

	int n = x_data.size();
	int degree = 3; // Cubic polynomial

	// Build design matrix A (Vandermonde matrix: n x 4) and vector y
	Eigen::MatrixXd A(n, degree + 1);
	Eigen::VectorXd y(n);

	for (int i = 0; i < n; ++i)
	{
		double x = x_data[i];
		A(i, 0) = 1.0;
		A(i, 1) = x;
		A(i, 2) = x * x;
		A(i, 3) = x * x * x;
		y(i) = y_data[i];
	}

	// Solve least-squares problem: A * c = y using QR decomposition
	Eigen::VectorXd c = A.colPivHouseholderQr().solve(y);

	std::cout << "Cubic coefficients (c0, c1, c2, c3):\n"
			  << c << std::endl;

	return 0;
}
