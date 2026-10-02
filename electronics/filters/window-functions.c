/*

https://www.recordingblogs.com/wiki/coherent-gain
https://www.gaussianwaves.com/2020/09/window-function-figure-of-merits/

*/

#define _GNU_SOURCE
#include <stdio.h>
#include <math.h>
#include <complex.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

double
barlett_hann(double k, double N)
{
	double a = 0.62;
	double b = 0.48 * fabs(k / (N - 1) - 0.5);
	double c = 0.38 * cos((2 * M_PI * k) / (N - 1));
	return a - b - c;
}

double
blackman(double k, double N)
{
	double a = 0.42;
	double b = 0.5 * cos((2 * M_PI * k) / (N - 1));
	double c = 0.08 * cos((4 * M_PI * k) / (N - 1));
	return a - b + c;
}

double
blackman_harris(double k, double N)
{
	double a = 0.35875;
	double b = 0.48829 * cos((2 * M_PI * k) / (N - 1));
	double c = 0.14128 * cos((4 * M_PI * k) / (N - 1));
	double d = 0.01168 * cos((6 * M_PI * k) / (N - 1));
	return a - b + c - d;
}

double
blackman_nuttall(double k, double N)
{
	double a = 0.3635819;
	double b = 0.4891775 * cos((2 * M_PI * k) / (N - 1));
	double c = 0.1365995 * cos((4 * M_PI * k) / (N - 1));
	double d = 0.0106411 * cos((6 * M_PI * k) / (N - 1));
	return a - b + c - d;
}

double
bohman(double k, double N)
{
	double M = (N - 1) / 2;
	double a = 1 - fabs(k / M - 1);
	double b = cos(M_PI * fabs(k / M - 1));
	double c = 1 / M_PI;
	double d = sin(M_PI * fabs(k / M - 1));
	return a * b + c * d;
}

double
flat_top(double k, double N)
{
	double a = 0.21557895;
	double b = 0.41663158 * cos((2 * M_PI * k) / (N - 1));
	double c = 0.277263158 * cos((4 * M_PI * k) / (N - 1));
	double d = 0.083578947 * cos((6 * M_PI * k) / (N - 1));
	double e = 0.006947368 * cos((8 * M_PI * k) / (N - 1));
	return a - b + c - d + e;
}

double
gaussian(double k, double N, double s)
{
	double M = (N - 1) / 2;
	double t = (k - M) / (s * M);
	return exp(-0.5 * t * t);
}

double
gaussian_03(double k, double N)
{
	return gaussian(k, N, 0.3);
}

double
gaussian_05(double k, double N)
{
	return gaussian(k, N, 0.5);
}

double
gaussian_07(double k, double N)
{
	return gaussian(k, N, 0.7);
}

double
approx_confined_gaussian(double k, double N, double s)
{
	double a = gaussian(k, N, s);
	double b0 = gaussian(-0.5, N, s);
	double b1 = gaussian(k + N, N, s);
	double b2 = gaussian(k - N, N, s);
	double c0 = gaussian(-0.5 + N, N, s);
	double c1 = gaussian(-0.5 - N, N, s);
	return a - (b0 * (b1 + b2)) / (c0 + c1);
}

double
approx_confined_gaussian_03(double k, double N)
{
	return approx_confined_gaussian(k, N, 0.3);
}

double
approx_confined_gaussian_05(double k, double N)
{
	return approx_confined_gaussian(k, N, 0.5);
}

double
approx_confined_gaussian_07(double k, double N)
{
	return approx_confined_gaussian(k, N, 0.7);
}

double
generalized_normal(double k, double N, double s, double a)
{
	double M = (N - 1) / 2;
	double p = pow(fabs((k - M) / (s * M)), a);
	return exp(-0.5 * p);
}

double
generalized_normal_2(double k, double N)
{
	return generalized_normal(k, N, 0.5, 2);
}

double
generalized_normal_4(double k, double N)
{
	return generalized_normal(k, N, 0.5, 4);
}

double
generalized_normal_6(double k, double N)
{
	return generalized_normal(k, N, 0.5, 6);
}

double
hamming(double k, double N)
{
	double a = 0.54;
	double b = 0.46 * cos((2 * M_PI * k) / (N - 1));
	return a - b;
}

double
hann(double k, double N)
{
	return 0.5 * (1 - cos((2 * M_PI * k) / (N - 1)));
}

double
hann_poisson(double k, double N, double a)
{
	double M = (N - 1) / 2;
	double x = 0.5 * (1 - cos(M_PI * k / M));
	double y = exp(-a * fabs(k - M) / M);
	return x * y;
}

double
hann_poisson03(double k, double N)
{
	return hann_poisson(k, N, 0.3);
}

double
hann_poisson05(double k, double N)
{
	return hann_poisson(k, N, 0.5);
}

double
hann_poisson07(double k, double N)
{
	return hann_poisson(k, N, 0.7);
}

double
lanczos(double k, double N)
{
	if (k == (N - 1) / 2)
		return 1;

	double p = M_PI * ((2 * k) / (N - 1) - 1);
	return sin(p) / p;
}

double
kaiser_bessel(double k, double N)
{
	double a = 0.402;
	double b = 0.498 * cos((2 * M_PI * k) / (N - 1));
	double c = 0.098 * cos((4 * M_PI * k) / (N - 1));
	double d = 0.001 * cos((6 * M_PI * k) / (N - 1));
	return a - b + c - d;
}

double
parzen(double k, double N)
{
	double M = (N - 1) / 2;
	double p = fabs(k - M) / N;
	if (fabs(k - M) <= M)
		return 1 - (6 * p * p) + (6 * p * p * p);
	return 2 * (1 - p) * (1 - p) * (1 - p);
}

double
poisson(double k, double N, double a)
{
	double M = (N - 1) / 2;
	return exp(-a * fabs(k - M) / M);
}

double
poisson_02(double k, double N)
{
	return poisson(k, N, 0.2);
}

double
poisson_05(double k, double N)
{
	return poisson(k, N, 0.5);
}

double
poisson_08(double k, double N)
{
	return poisson(k, N, 0.8);
}

double
power_of_cosine(double k, double N, double a)
{
	return pow(cos((M_PI * k) / (N - 1) - (M_PI / 2)), a);
}

double
power_of_cosine_1(double k, double N)
{
	return power_of_cosine(k, N, 1);
}

double
power_of_cosine_2(double k, double N)
{
	return power_of_cosine(k, N, 2);
}

double
power_of_cosine_3(double k, double N)
{
	return power_of_cosine(k, N, 3);
}

double
rectangular(double, double)
{
	return 1;
}

double
sine(double k, double N)
{
	return sin((M_PI * k) / (N - 1));
}

double
triangular(double k, double N)
{
	double M = (N - 1) / 2;
	return 1 - (fabs(k - M) / M);
}

double
tukey(double k, double N, double a)
{
	double M = (N - 1) / 2;
	double r = fabs(k - M);
	if (r < a * M)
		return 1;
	return 0.5 * (1 + cos((M_PI * (r - (a * M))) / ((1 - a) * M)));
}

double
tukey_03(double k, double N)
{
	return tukey(k, N, 0.3);
}

double
tukey_05(double k, double N)
{
	return tukey(k, N, 0.5);
}

double
tukey_07(double k, double N)
{
	return tukey(k, N, 0.7);
}

double
welch(double k, double N)
{
	double a = 1;
	double b = (k - (N - 1) / 2) / ((N + 1) / 2);
	return a - (b * b);
}

void
gen_window(double *a, size_t n, double (*f)(double, double))
{
	size_t i;

	for (i = 0; i < n; i++)
		a[i] = f(i, n);
}

/*

In time-domain, windowing a signal reduces the amplitude of the signal at its boundaries (Figure 4).
This causes a reduction in the amplitude of the spectral component when FFT used to visualize the signal in frequency domain.
This reduction in amplitude in the spectral components is characterized as coherent power gain (which actually is a loss).
So, when FFT is computed on a windowed signal, in order to compensate for the loss in the amplitude, we simply add the coherent power gain to the FFT output.

*/
double
coherent_gain(double *a, size_t n)
{
	size_t i;
	double s;

	s = 0;
	for (i = 0; i < n; i++)
		s += a[i];
	return s / n;
}

double
scalloping_loss(double *a, size_t n)
{
	double complex x, y;
	size_t i;

	x = y = 0;
	for (i = 0; i < n; i++) {
		x += a[i] * cexp(-M_PI * I * (i * 1.0 / n));
		y += a[i];
	}
	return cabs(x) / creal(y);
}

void
test(const char *name, double (*f)(double, double))
{
	double a[1024];
	size_t n;

	n = nelem(a);
	gen_window(a, n, f);

	double ch = coherent_gain(a, n);
	double sl = scalloping_loss(a, n);
	double sl_db = 20 * log10(sl);

	printf("%s\n", name);
	printf("Coherent Gain (Average) %.2f\n", ch);
	printf("Scalloping Loss         %.2f %.2f\n", sl, sl_db);
	printf("\n");
}

int
main(void)
{
	static const struct {
		char name[64];
		double (*window)(double, double);
	} tab[] = {
	    {"Barlett Hann", barlett_hann},
	    {"Blackman", blackman},
	    {"Blackman Harris", blackman_harris},
	    {"Blackman Nuttall", blackman_nuttall},
	    {"Bohman", bohman},
	    {"Flat Top", flat_top},
	    {"Gaussian (s=0.3)", gaussian_03},
	    {"Gaussian (s=0.5)", gaussian_05},
	    {"Gaussian (s=0.7)", gaussian_07},
	    {"Approximate Confined Gaussian (s=0.3)", approx_confined_gaussian_03},
	    {"Approximate Confined Gaussian (s=0.5)", approx_confined_gaussian_05},
	    {"Approximate Confined Gaussian (s=0.7)", approx_confined_gaussian_07},
	    {"Generalized Normal (a=2)", generalized_normal_2},
	    {"Generalized Normal (a=4)", generalized_normal_4},
	    {"Generalized Normal (a=6)", generalized_normal_6},
	    {"Hamming", hamming},
	    {"Hann", hann},
	    {"Hann Poisson (a=0.3)", hann_poisson03},
	    {"Hann Poisson (a=0.5)", hann_poisson05},
	    {"Hann Poisson (a=0.7)", hann_poisson07},
	    {"Lanczos", lanczos},
	    {"Kaiser Bessel", kaiser_bessel},
	    {"Parzen", parzen},
	    {"Poisson (a=0.2)", poisson_02},
	    {"Poisson (a=0.5)", poisson_05},
	    {"Poisson (a=0.8)", poisson_08},
	    {"Power of cosine (a=1.0)", power_of_cosine_1},
	    {"Power of cosine (a=2.0)", power_of_cosine_2},
	    {"Power of cosine (a=3.0)", power_of_cosine_3},
	    {"Rectangular", rectangular},
	    {"Sine", sine},
	    {"Triangular", triangular},
	    {"Tukey (a=0.3)", tukey_03},
	    {"Tukey (a=0.5)", tukey_05},
	    {"Tukey (a=0.7)", tukey_07},
	    {"Welch", welch},
	};

	size_t i;

	for (i = 0; i < nelem(tab); i++)
		test(tab[i].name, tab[i].window);

	return 0;
}
