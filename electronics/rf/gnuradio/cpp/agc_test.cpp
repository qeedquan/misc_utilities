/*

https://wiki.gnuradio.org/index.php/AGC

*/

#include "common.h"

struct AGC
{
	float R; // update rate
	float r; // reference
	float K; // initial gain
	float M; // max gain

	float scale(float x);
};

/*

Like a P controller
Uses a reference point to see find the error and use that to adjust the gain
As we are closer to the setpoint, the gain is smaller.

*/

float AGC::scale(float x)
{
	auto v = K * x;
	K += R * (r - fabs(v));
	if (M > 0 && K > M)
		K = M;
	return v;
}

/*

The AGC has a region of convergence, if the parameters are not picked carefully, the output can be unbounded.
Generally the parameters should be <= 1 to prevent blowup.

What it does numerically is that the AGC tries to keep the error bound small relative to the reference point r.
The error tries to be in the range [r - |r/2|, r + |r/2|] even if the input bound grows larger to a point.
Although if input exceeds the reference point too much, then the error will go off to infinity 

*/

void test_roc(float R, float r, float K, float M, float xr)
{
	gr::analog::kernel::agc_ff grc(R, r, K, M);
	AGC agc = { R, r, K, M };

	printf("R=%.6f r=%.6f K=%.6f M=%.6f\n", R, r, K, M);
	float x0 = -(r * xr);
	float x1 = r * xr;
	float dx = 1e-1;

	float mi = FLT_MAX;
	float ma = -FLT_MAX;
	for (float x = x0; x <= x1; x += dx)
	{
		float v0 = x;
		float v1 = x;
		for (auto i = 0; i < 1000; i++)
		{
			v0 = grc.scale(v0);
			v1 = agc.scale(v1);
			auto d = fabs(v0 - v1);
			if (d >= 1e-4)
			{
				printf("MISMATCH %f %f\n", v0, v1);
			}
		}

		mi = min(mi, abs(r - v0));
		ma = max(ma, abs(r - v0));
	}
	printf("Region: %f %f Min Error: %f Max Error: %f\n", x0, x1, mi, ma);
	printf("\n");
}

int main()
{
	test_roc(1e-4, 13, 1, 1, 100);
	test_roc(1e-4, 100, 1, 1, 100);

	test_roc(1e-4, 1000, 0.25, 1, 10);

	test_roc(1e-4, 500, 0.5, 1, 60);
	test_roc(1e-4, 500, 1, 1, 200);

	test_roc(1, 100, 0.5, 1, 2);
	test_roc(1, 34, 2, 5, 2);

	return 0;
}