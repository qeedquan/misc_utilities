// https://en.wikipedia.org/wiki/Row-_and_column-major_order
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void
cons(size_t r, size_t c, int M[r][c], int v)
{
	size_t i, j;

	for (i = 0; i < r; i++) {
		for (j = 0; j < c; j++)
			M[i][j] = v;
	}
}

void
mulsq(size_t n, int A[n][n], int B[n][n], int C[n][n])
{
	size_t i, j, k;

	cons(n, n, C, 0);
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			for (k = 0; k < n; k++)
				C[i][j] += A[i][k] * B[k][j];
		}
	}
}

void
fill(size_t r, size_t c, int M[r][c])
{
	size_t i, j;

	for (i = 0; i < r; i++) {
		for (j = 0; j < c; j++)
			M[i][j] = i * r + j;
	}
}

void
rands(size_t r, size_t c, int M[r][c])
{
	size_t i, j;

	for (i = 0; i < r; i++) {
		for (j = 0; j < c; j++)
			M[i][j] = rand() % 100;
	}
}

void
transpose(size_t r, size_t c, int M[r][c], int N[c][r])
{
	size_t i, j;

	for (i = 0; i < r; i++) {
		for (j = 0; j < c; j++)
			N[j][i] = M[i][j];
	}
}

void
row2col(size_t r, size_t c, int M[r][c], int N[r][c])
{
	size_t i, j;
	int *p;

	p = &N[0][0];
	for (i = 0; i < c; i++) {
		for (j = 0; j < r; j++)
			*p++ = M[j][i];
	}
}

void
dump(size_t r, size_t c, int M[r][c])
{
	size_t i, j;

	for (i = 0; i < r; i++) {
		for (j = 0; j < c; j++)
			printf("%d ", M[i][j]);
		printf("\n");
	}
	printf("\n");
}

void
dump_raw(void *b, size_t n)
{
	size_t i;
	int *p;

	p = b;
	for (i = 0; i < n; i++) {
		printf("%d ", p[i]);
		if ((i & 15) == 15)
			printf("\n");
	}
	printf("\n");
}

void
test_cycle(size_t r, size_t c)
{
	int M[r][c], N[r][c], T[r][c];
	size_t i;

	fill(r, c, M);

	// if we keep converting row to columns, we will eventually
	// get back the original matrix we start, figure out how many
	// cycles that take
	memcpy(N, M, sizeof(M));
	for (i = 1;; i++) {
		row2col(r, c, N, T);
		memcpy(N, T, sizeof(T));
		if (!memcmp(M, N, sizeof(N)))
			break;
	}
	printf("%zux%zu took %zu cycles\n", r, c, i);
}

void
test_transpose(size_t r, size_t c, int n)
{
	int M[r][c];
	int A[r][c], B[r][c];
	int X[r][c], Y[c][r];
	int i, m;

	if (r == 0 || c == 0)
		return;

	fill(r, c, M);
	memcpy(A, M, sizeof(M));
	memcpy(X, M, sizeof(M));
	for (i = 0; i < n; i++) {
		row2col(r, c, A, B);
		if (!(i & 1)) {
			transpose(r, c, X, Y);
			m = memcmp(B, Y, sizeof(Y));
		} else {
			transpose(c, r, Y, X);
			m = memcmp(B, X, sizeof(X));
		}
		memcpy(A, B, sizeof(B));

		if (m) {
			printf("#%d %zux%zu mismatch\n", i, r, c);
			dump(r, c, B);
			if (!(i & 1))
				dump(c, r, Y);
			else
				dump(r, c, X);
		}
	}
}

void
test_square_transpose(size_t n)
{
	int A[n][n], B[n][n], C[n][n], D[n][n], E[n][n], F[n][n], G[n][n], H[n][n];

	rands(n, n, A);
	rands(n, n, B);

	// A * B
	mulsq(n, A, B, C);

	// D = transpose(A*B)
	transpose(n, n, C, D);

	// E = transpose(B)
	transpose(n, n, B, E);

	// F = transpose(A)
	transpose(n, n, A, F);

	// G = transpose(B)*transpose(A)
	mulsq(n, E, F, G);

	// same ops as above but replace transpose with row2col
	mulsq(n, A, B, C);
	row2col(n, n, C, D);
	row2col(n, n, B, E);
	row2col(n, n, A, F);
	mulsq(n, E, F, H);
	if (memcmp(H, G, sizeof(G))) {
		printf("%zux%zu transpose mismatch\n", n, n);
		dump(n, n, G);
		dump(n, n, H);
	}
}

int
main(void)
{
	int i, j;

	srand(time(NULL));

	// 1x1 matrix takes 1 cycle since its only one element
	// every other square matrix takes 2 cycles
	// this implies that row2col is its own inverse for square matrix
	printf("# Test Cycle\n");
	for (i = 1; i <= 100; i++)
		test_cycle(i, i);

	// non-square matrices have a weird number of cycles
	test_cycle(2, 3);
	test_cycle(4, 39);
	test_cycle(54, 100);
	test_cycle(320, 30);
	printf("\n");

	// transposing a matrix and converting row to column are the same operation
	// at the first time, afterwards they are not the same operation anymore
	// unless they are square matrices since row2col is its own inverse at that point

	// one common mistake is doing the following
	// transpose(r, c, X, Y);
	// transpose(r, c, Y, X);
	// if we do it this way, the transpose operation will be exactly the same as row2col
	// always, but the issue is that the second transpose should be passed as (c, r) instead of (r, c)

	printf("# Test Transpose\n");
	for (i = 0; i <= 4; i++)
		for (j = 0; j <= 4; j++)
			test_transpose(i, j, 5);

	// for square matrices, some identities should work, ie
	// transpose(A*B) = transpose(B)*transpose(A)
	// row2col(A*B) == row2col(A)*row2col(B)
	printf("# Test Square Transpose\n");
	for (i = 1; i <= 100; i++)
		test_square_transpose(i);

	return 0;
}
