/*

https://www.mikroe.com/ebooks/digital-filter-design/window-functions
https://dsp.stackexchange.com/questions/31066/how-many-taps-does-an-fir-filter-need/31077

*/

#include "common.h"

vector<float> gen_hamming_window(int ntaps)
{
	vector<float> c;
	for (auto i = 0; i < ntaps; i++)
	{
		auto a0 = 0.54;
		auto a1 = 0.46;
		auto n = ntaps - 1;
		auto v = a0 -
				 a1 * cos((TAU * i) / n);
		c.push_back(v);
	}
	return c;
}

vector<float> gen_hann_window(int ntaps)
{
	vector<float> c;
	for (auto i = 0; i < ntaps; i++)
	{
		auto a0 = 0.5;
		auto a1 = 0.5;
		auto n = ntaps - 1;
		auto v = a0 -
				 a1 * cos((TAU * i) / n);
		c.push_back(v);
	}
	return c;
}

vector<float> gen_blackman_window(int ntaps)
{
	vector<float> c;
	for (auto i = 0; i < ntaps; i++)
	{
		auto alpha = 0.16;
		auto a0 = (1 - alpha) / 2;
		auto a1 = 0.5;
		auto a2 = alpha / 2;
		auto n = ntaps - 1;
		auto v = a0 -
				 a1 * cos((TAU * i) / n) +
				 a2 * cos((2 * TAU * i) / n);
		c.push_back(v);
	}
	return c;
}

vector<float> gen_rectangular_window(int ntaps)
{
	vector<float> c;
	for (auto i = 0; i < ntaps; i++)
		c.push_back(1);
	return c;
}

vector<float> gen_blackman_harris_window(int ntaps)
{
	vector<float> c;
	for (auto i = 0; i < ntaps; i++)
	{
		auto a0 = 0.35875;
		auto a1 = 0.48829;
		auto a2 = 0.14128;
		auto a3 = 0.01168;
		auto n = ntaps - 1;
		auto v = a0 -
				 a1 * cos((TAU * i) / n) +
				 a2 * cos((2 * TAU * i) / n) -
				 a3 * cos((3 * TAU * i) / n);
		c.push_back(v);
	}
	return c;
}

vector<float> gen_bartlett_window(int ntaps)
{
	vector<float> c;
	for (auto i = 0; i < ntaps; i++)
	{
		auto n = ntaps - 1;
		auto a0 = 2.0 / n;
		auto a1 = n / 2.0;
		auto a2 = i - a1;
		auto v = a0 * (a1 - fabs(a2));
		c.push_back(v);
	}
	return c;
}

vector<float> gen_flattop_window(int ntaps)
{
	vector<float> c;
	for (auto i = 0; i < ntaps; i++)
	{
		auto a0 = 0.21557903450773605;
		auto a1 = 0.41606753659993057;
		auto a2 = 0.2780969545149795;
		auto a3 = 0.08364466538900159;
		auto a4 = 0.006036212966216609;
		auto n = ntaps - 1;
		auto v = a0 -
				 a1 * cos((TAU * i) / n) +
				 a2 * cos((2 * TAU * i) / n) -
				 a3 * cos((3 * TAU * i) / n) +
				 a4 * cos((4 * TAU * i) / n);
		c.push_back(v);
	}
	return c;
}

void gen_window_coeffs(window::win_type type, int ntaps, const float[], vector<float> &coeffs, vector<float> &xcoeffs)
{
	switch (type)
	{
	case window::WIN_HAMMING:
		coeffs = window::hamming(ntaps);
		xcoeffs = gen_hamming_window(ntaps);
		break;
	case window::WIN_HANN:
		coeffs = window::hann(ntaps);
		xcoeffs = gen_hann_window(ntaps);
		break;
	case window::WIN_BLACKMAN:
		coeffs = window::blackman(ntaps);
		xcoeffs = gen_blackman_window(ntaps);
		break;
	case window::WIN_RECTANGULAR:
		coeffs = window::rectangular(ntaps);
		xcoeffs = gen_rectangular_window(ntaps);
		break;
	case window::WIN_KAISER:
		coeffs = window::kaiser(ntaps, TAU);
		break;
	case window::WIN_BLACKMAN_HARRIS:
		coeffs = window::blackmanharris(ntaps);
		xcoeffs = gen_blackman_harris_window(ntaps);
		break;
	case window::WIN_BARTLETT:
		coeffs = window::bartlett(ntaps);
		xcoeffs = gen_bartlett_window(ntaps);
		break;
	case window::WIN_FLATTOP:
		coeffs = window::flattop(ntaps);
		xcoeffs = gen_flattop_window(ntaps);
		break;
	}
}

/*

this is gnuradio internal function to calculate the number of taps
based on sampling frequency, the transition width, and attenuation in decibels

*/
int grc_compute_ntaps(double sampling_freq, double transition_width, double attenuation_db)
{
	auto alpha = attenuation_db / 22.0;
	auto ntaps = (int)((alpha * sampling_freq) / transition_width);
	if ((ntaps & 1) == 0)
		ntaps++;
	return ntaps;
}

/*

from bellanger digital processing of signals book
2/3 * log10(1/(10*s1*s2)) * fs/df
fs - sampling rate/freq (hz)
df - transition width (db)
s1 - ripple in passband (how much of original amplitude can afford to vary) (in percentage, say 0.1% in amplitude variation is 1e-4)
s2 - the suppression in the (convert db to gain, example: -60db is 1e-3)

if we want to match grc parameterization we have the following equation
s = s1*s2
A = 2/3
B = 1/10
C = 1/22
A*log10(B/s) = C*a

s = B*pow10(-C/A * a)

*/

int bellanger_compute_ntaps(double sampling_freq, double transition_width, double attenuation_db)
{
	auto A = 2.0 / 3;
	auto B = 1.0 / 10;
	auto C = 1.0 / 22;
	auto sigma = B * pow(10, -C / A * attenuation_db);
	auto alpha = A * log10(B / sigma);
	auto ntaps = (int)(alpha * sampling_freq / transition_width);
	if ((ntaps & 1) == 0)
		ntaps++;
	return ntaps;
}

void get_window_info(int ntaps, double sampling_frequency, double transition_width)
{
	for (auto w : filters)
	{
		float args[8] = { 0 };

		vector<float> coeffs, xcoeffs;
		gen_window_coeffs(w.type, ntaps, args, coeffs, xcoeffs);

		auto max_attenuation_db = window::max_attenuation(w.type);
		auto grc_ntaps = grc_compute_ntaps(sampling_frequency, transition_width, max_attenuation_db);
		auto bellanger_ntaps = bellanger_compute_ntaps(sampling_frequency, transition_width, max_attenuation_db);

		printf("%s\n", w.name);
		printf("Sampling Frequency %.6f\n", sampling_frequency);
		printf("Transition Width %.6f\n", transition_width);
		printf("Max attenuation (DB): %.6f\n", max_attenuation_db);
		printf("Window Size (Number of taps) Parameter %d GRC = %d Bellanger = %d)\n", ntaps, grc_ntaps, bellanger_ntaps);
		for (auto i = 0u; i < coeffs.size(); i++)
		{
			printf("%d %.6f", i, coeffs[i]);
			if (i < xcoeffs.size())
			{
				printf(" %.6f", xcoeffs[i]);
				assert(fabs(coeffs[i] - xcoeffs[i]) < 1e-6);
			}
			printf("\n");
		}
		printf("\n");
	}
}

int main()
{
	get_window_info(32, 48 * 1000, 1052);
	return 0;
}
