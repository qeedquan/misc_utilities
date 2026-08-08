// https://en.wikipedia.org/wiki/Stochastic_matrix
#include <array>
#include <cstdio>
#include <cmath>

using std::array;

template <size_t N> using Matrix = double[N][N];

// right stochastic if all rows sum to 1
// left stochastic if all columns sum to 1
// doubly stochastic if each row and columns sum to 1
template <size_t N>
int classify(Matrix<N> m)
{
	auto ls = true;
	auto rs = true;
	for (auto n = 0; n < N; n++)
	{
		for (auto i = 0; i < N; i++)
		{
			auto r = 0.0;
			auto c = 0.0;
			for (auto j = 0; j < N; j++)
			{
				if (m[i][j] < 0)
					return -1;

				r += m[i][j];
				c += m[j][i];
			}

			if (fabs(r-1.0) > 1e-6)
				rs = false;
			if (fabs(c-1.0) > 1e-6)
				ls = false;
			if (!rs && !ls)
				return -1;
		}
	}

	if (rs && ls)
		return 2;
	else if (ls)
		return 1;
	else if (rs)
		return 0;
	
	return -1;
}

int main()
{
	Matrix<2> m1 =
	{
		{0.5, 0.5},
		{0.5, 0.5}
	};
	printf("%d\n", classify(m1));
	
	Matrix<3> m2 =
	{
		{0.6, 0.1, 0.3},
		{0.3, 0.8, 0.4},
		{0.1, 0.1, 0.3},
	};
	printf("%d\n", classify(m2));

	Matrix<4>m3 =
	{
		{0.6, 0.1, 0.1, 0.2},
		{0.3, 0.3, 0.2, 0.2},
		{0.1, 0.85, 0.025, 0.025},
		{0.6, 0.1, 0.1, 0.2},
	};
	printf("%d\n", classify(m3));

	return 0;
}
