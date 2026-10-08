#ifndef _COMMON_H_
#define _COMMON_H_

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <vector>

#include <gnuradio/io_signature.h>
#include <gnuradio/digital/constellation.h>
#include <gnuradio/top_block.h>
#include <gnuradio/analog/agc.h>
#include <gnuradio/analog/sig_source.h>
#include <gnuradio/analog/noise_source.h>
#include <gnuradio/analog/quadrature_demod_cf.h>
#include <gnuradio/blocks/file_sink.h>
#include <gnuradio/blocks/vector_sink.h>
#include <gnuradio/blocks/head.h>
#include <gnuradio/filter/firdes.h>
#include <gnuradio/filter/fir_filter.h>
#include <gnuradio/filter/fir_filter_blk.h>
#include <gnuradio/fft/window.h>
#include <gnuradio/math.h>

#define TAU (2 * M_PI)

using namespace std;
using namespace gr;
using namespace gr::analog;
using namespace gr::digital;
using namespace gr::blocks;
using namespace gr::fft;
using namespace gr::filter;

struct Filter
{
	char name[32];
	window::win_type type;
};

static const Filter filters[] = {
	{ "Hamming", window::WIN_HAMMING },
	{ "Hann", window::WIN_HANN },
	{ "Blackman", window::WIN_BLACKMAN },
	{ "Rectangular", window::WIN_RECTANGULAR },
	{ "Kaiser", window::WIN_KAISER },
	{ "Blackman_Harris", window::WIN_BLACKMAN_HARRIS },
	{ "Bartlett", window::WIN_BARTLETT },
	{ "Flat_Top", window::WIN_FLATTOP },
};

template <typename T>
vector<T> pad_array(vector<T> &in, T val, size_t padlen, bool end)
{
	auto len = in.size();

	vector<T> out;
	out.resize(len + padlen);
	memcpy(&out[(end) ? 0 : padlen], &in[0], len * sizeof(in[0]));

	for (auto i = 0u; i < padlen; i++)
	{
		auto j = i;
		if (end)
			j += len;

		out[j] = val;
	}
	return out;
}

auto pad_array_f = pad_array<float>;
auto pad_array_c = pad_array<gr_complex>;

#endif
