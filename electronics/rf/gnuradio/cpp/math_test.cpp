/*

http://web.mit.edu/6.02/www/s2012/handouts/14.pdf
https://wirelesspi.com/fsk-demodulation-in-gnu-radio/

*/

#include "common.h"

void test_wave_source()
{
	gr_waveform_t waveforms[] = {
		GR_CONST_WAVE,
		GR_SIN_WAVE,
		GR_COS_WAVE,
		GR_SQR_WAVE,
		GR_TRI_WAVE,
		GR_SAW_WAVE,
	};

	for (auto wave : waveforms)
	{
		auto samplerate = 10;
		auto freq = 1;
		auto gain = 13.5;
		auto off = 0.0;

		char str[128];
		sprintf(str, "source_%d.bin", wave);

		auto tb = make_top_block("Source");
		auto source = sig_source_f::make(samplerate, wave, freq, gain, off);
		auto limiter = head::make(sizeof(float), samplerate);
		auto vector_sink = vector_sink_f::make(1);
		auto file_sink = file_sink::make(sizeof(float), str, false);

		// we can redirect output to both a vector and a file
		tb->hier_block2::connect(source, 0, limiter, 0);
		tb->hier_block2::connect(limiter, 0, file_sink, 0);
		tb->hier_block2::connect(limiter, 0, vector_sink, 0);
		tb->run();
	}
}

void test_binary_slicer()
{
	// a binary slicer returns 1 for >= 0, 0 for < 0
	for (auto i = -100; i <= 100; i++)
	{
		assert(binary_slicer(i) == (i < 0) ? 0 : 1);
	}
}

void test_quadrature_demod(double samplerate, double gain)
{
	auto tb = make_top_block("Quadrature Demod");
	auto source = noise_source_c::make(GR_UNIFORM, gain);
	auto limiter = head::make(sizeof(gr_complex), samplerate);
	auto in_sink = vector_sink_c::make(1);
	auto out_sink = vector_sink_f::make(1);
	auto quad_demod = quadrature_demod_cf::make(gain);

	tb->hier_block2::connect(source, 0, limiter, 0);
	tb->hier_block2::connect(limiter, 0, quad_demod, 0);
	tb->hier_block2::connect(limiter, 0, in_sink, 0);
	tb->hier_block2::connect(quad_demod, 0, out_sink, 0);
	tb->run();

	// quadrature demodulation is
	// complex -> float
	// out[i] = in[i] * conj(in[i-1])
	// Arg(out[i]) gives us the modulation frequency
	vector<gr_complex> vecs;
	gr_complex u = 0;
	for (auto v : in_sink->data())
	{
		auto t = v * conj(u);
		vecs.push_back(gain * atan2(t.imag(), t.real()));
		u = v;
	}

	auto dat = out_sink->data();
	for (auto i = 0ull; i < dat.size(); i++)
	{
		assert(fabs(dat[i] - vecs[i].real()) < 1e-5);
	}
}

int main()
{
	test_wave_source();
	test_binary_slicer();
	test_quadrature_demod(20, 5);
	return 0;
}