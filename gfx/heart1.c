#include <stdio.h>
#include <math.h>

double
at(double x, double y)
{
	double A, B;
	double X, Y;

	X = x * 0.05;
	Y = y * 0.1;

	A = pow(X, 2) + pow(Y, 2) - 1;
	B = pow(X, 2) * pow(Y, 3);
	return pow(A, 3) - B;
}

void
heart(int x0, int y0, int x1, int y1)
{
	int x, y;

	for (y = y1; y >= y0; y--) {
		for (x = x0; x <= x1; x++) {
			if (at(x, y) <= 0)
				printf("%s", u8"❤️");
			else
				printf("  ");
		}
		printf("\n");
	}
}

int
main()
{
	heart(-30, -11, 30, 12);
	return 0;
}
