/*

http://rinterested.github.io/statistics/covariance.html
https://en.wikipedia.org/wiki/Centering_matrix

*/

#include <stdio.h>
#include <assert.h>

void
dump1d(size_t n, double X[n])
{
	size_t i;

	for (i = 0; i < n; i++)
		printf("% 12.6f ", X[i]);
	printf("\n");
}

void
dump2d(size_t r, size_t c, double X[r][c])
{
	size_t i;

	for (i = 0; i < r; i++)
		dump1d(c, X[i]);
}

void
eye(size_t n, double X[n][n])
{
	size_t i, j;

	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++)
			X[i][j] = 0;
		X[i][i] = 1;
	}
}

void
ones(size_t n, double X[n][n])
{
	size_t i, j;

	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++)
			X[i][j] = 1;
	}
}

void
scale(size_t r, size_t c, double X[r][c], double s)
{
	size_t i, j;

	for (i = 0; i < r; i++) {
		for (j = 0; j < c; j++)
			X[i][j] *= s;
	}
}

void
sub(size_t r, size_t c, double X[r][c], double Y[r][c], double Z[r][c])
{
	size_t i, j;

	for (i = 0; i < r; i++) {
		for (j = 0; j < c; j++)
			Z[i][j] = X[i][j] - Y[i][j];
	}
}

void
centers(size_t n, double X[n][n])
{
	double I[n][n], O[n][n];

	eye(n, I);
	ones(n, O);
	scale(n, n, O, 1.0 / n);
	sub(n, n, I, O, X);
}

double
mu(size_t r, size_t c, double X[r][c], size_t l)
{
	size_t i;
	double m;

	m = 0.0;
	for (i = 0; i < r; i++)
		m += X[i][l];
	return m / r;
}

double
sigma(size_t r, size_t xc, size_t yc, double X[r][xc], double Y[r][yc], double xm, double ym, size_t xi, size_t yi)
{
	size_t i;
	double s;

	s = 0.0;
	for (i = 0; i < r; i++)
		s += (X[i][xi] - xm) * (Y[i][yi] - ym);
	return s / (r - 1);
}

void
cov(size_t xr, size_t xc, size_t yr, size_t yc, double X[xr][xc], double Y[yr][yc], double xm[xc], double ym[yc], double Z[xc][yc])
{
	size_t i, j;

	assert(xr == yr);
	for (i = 0; i < xc; i++)
		xm[i] = mu(xr, xc, X, i);
	for (i = 0; i < yc; i++)
		ym[i] = mu(yr, yc, Y, i);

	for (i = 0; i < xc; i++) {
		for (j = 0; j <= i; j++)
			Z[i][j] = Z[j][i] = sigma(xr, xc, yc, X, Y, xm[i], ym[j], i, j);
	}
}

void
testcenter(void)
{
	double C[10][10];
	size_t i;

	for (i = 1; i <= 5; i++) {
		printf("Center Matrix [%zu, %zu]\n", i, i);
		centers(i, C);
		dump2d(i, i, C);
		printf("\n");
	}
	printf("\n");
}

void
testcov(size_t xr, size_t xc, size_t yr, size_t yc, double X[xr][xc], double Y[yr][yc])
{
	double Z[xc][yc];
	double xm[xc];
	double ym[yc];

	cov(xr, xc, yr, yc, X, Y, xm, ym, Z);
	printf("cov(X[%zu, %zu], Y[%zu, %zu]) = Z[%zu, %zu]\n", xr, xc, yr, yc, xc, yc);
	printf("X Means\n");
	dump1d(xc, xm);
	printf("\n");

	printf("Y Means\n");
	dump1d(yc, ym);
	printf("\n");

	printf("Covariance\n");
	dump2d(xc, yc, Z);
	printf("\n");
}

int
main(void)
{
	double X1[5][3] = {
	    {4.0, 2.0, 0.6},
	    {4.2, 2.1, 0.59},
	    {3.9, 2.0, 0.58},
	    {4.3, 2.1, 0.62},
	    {4.1, 2.2, 0.63},
	};
	double X2[5][3] = {
	    {64.0, 580.0, 29.0},
	    {66.0, 570.0, 33.0},
	    {68.0, 590.0, 37.0},
	    {69.0, 660.0, 46.0},
	    {73.0, 600.0, 55.0},
	};
	double X3[2][2] = {
	    {3.0, 7.0},
	    {2.0, 4.0},
	};
	double X4[3][3] = {
	    {2, 5, 9},
	    {3, 8, 7},
	    {4, 1, 5},
	};
	double X5[5][3] = {
	    {1, 8, 1},
	    {-4, 9, 3},
	    {5, 10, 4},
	    {7, 3, 5},
	    {8, 7, 6},
	};

	testcenter();
	testcov(5, 3, 5, 3, X1, X1);
	testcov(5, 3, 5, 3, X2, X2);
	testcov(2, 2, 2, 2, X3, X3);
	testcov(3, 3, 3, 3, X4, X4);
	testcov(5, 3, 5, 3, X5, X5);

	return 0;
}
