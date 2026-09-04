#include "common.h"

enum
{
	COSINE = 0,
	SINE,
};

vector<float> signal_source(int waveform, float samplerate, float frequency, float amplitude, float phase, size_t size)
{
	vector<float> out;

	auto dt = (TAU * frequency) / samplerate;
	auto t = phase;
	for (auto i = 0u; i < size; i++)
	{
		float val = 0;
		switch (waveform)
		{
		case COSINE:
			val = cos(t);
			break;
		case SINE:
			val = sin(t);
			break;
		}
		val *= amplitude;

		out.push_back(val);
		t += dt;
	}
	return out;
}

vector<float> random_source(float gain, size_t size)
{
	vector<float> out;
	for (auto i = 0u; i < size; i++)
		out.push_back(gain * drand48());
	return out;
}

vector<float> constant_source(float value, size_t size)
{
	vector<float> out;
	for (auto i = 0u; i < size; i++)
		out.push_back(value);
	return out;
}

void test_topblock(const char *name, window::win_type type, vector<float> &in, int decimation, float gain, float samplerate, float cutoff_low, float cutoff_high, float transition_width)
{
	char str[32];
	sprintf(str, "lpf_%s.bin", name);
	auto lpftb = make_top_block("Low Pass Filter");
	auto lpftaps = firdes::low_pass(gain, samplerate, cutoff_low, transition_width, (firdes::win_type)type);
	auto lpffir = fir_filter_fff::make(decimation, lpftaps);
	auto lpfhead = head::make(sizeof(in[0]), samplerate);
	auto lpffile = file_sink::make(sizeof(in[0]), str, false);
	auto lpfsrc = sig_source_f::make(0, GR_CONST_WAVE, 0, 0, 1);

	sprintf(str, "hpf_%s.bin", name);
	auto hpftb = make_top_block("High Pass Filter");
	auto hpftaps = firdes::high_pass(gain, samplerate, cutoff_high, transition_width, (firdes::win_type)type);
	auto hpffir = fir_filter_fff::make(decimation, hpftaps);
	auto hpfhead = head::make(sizeof(in[0]), samplerate);
	auto hpffile = file_sink::make(sizeof(in[0]), str, false);
	auto hpfsrc = sig_source_f::make(0, GR_CONST_WAVE, 0, 0, 1);

	sprintf(str, "lhpf_%s.bin", name);
	auto lhpftb = make_top_block("Low/High Pass Filter");
	auto lhpftaps_l = firdes::low_pass(gain, samplerate, cutoff_low, transition_width, (firdes::win_type)type);
	auto lhpftaps_h = firdes::high_pass(gain, samplerate, cutoff_high, transition_width, (firdes::win_type)type);
	auto lhpffir_l = fir_filter_fff::make(decimation, lhpftaps_l);
	auto lhpffir_h = fir_filter_fff::make(decimation, lhpftaps_h);
	auto lhpfhead = head::make(sizeof(in[0]), samplerate);
	auto lhpffile = file_sink::make(sizeof(in[0]), str, false);
	auto lhpfsrc = sig_source_f::make(0, GR_CONST_WAVE, 0, 0, 1);

	sprintf(str, "bpf_%s.bin", name);
	auto bpftb = make_top_block("Band Pass Filter");
	auto bpftaps = firdes::band_pass(gain, samplerate, cutoff_low, cutoff_high, transition_width, (firdes::win_type)type);
	auto bpffir = fir_filter_fff::make(decimation, bpftaps);
	auto bpfhead = head::make(sizeof(in[0]), samplerate);
	auto bpffile = file_sink::make(sizeof(in[0]), str, false);
	auto bpfsrc = sig_source_f::make(0, GR_CONST_WAVE, 0, 0, 1);

	lpftb->hier_block2::connect(lpfsrc, 0, lpffir, 0);
	lpftb->hier_block2::connect(lpfhead, 0, lpffile, 0);
	lpftb->hier_block2::connect(lpffir, 0, lpfhead, 0);
	lpftb->start();
	lpftb->wait();

	hpftb->hier_block2::connect(hpfsrc, 0, hpffir, 0);
	hpftb->hier_block2::connect(hpfhead, 0, hpffile, 0);
	hpftb->hier_block2::connect(hpffir, 0, hpfhead, 0);
	hpftb->start();
	hpftb->wait();

	// cascading low and high pass filters like this is commutative
	// doing high pass and then low pass gives the same as doing low and then high
	// however, it does not give the same result as a bandpass filter below
	lhpftb->hier_block2::connect(lhpfsrc, 0, lhpffir_l, 0);
	lhpftb->hier_block2::connect(lhpfhead, 0, lhpffile, 0);
	lhpftb->hier_block2::connect(lhpffir_l, 0, lhpffir_h, 0);
	lhpftb->hier_block2::connect(lhpffir_h, 0, lhpfhead, 0);
	lhpftb->start();
	lhpftb->wait();

	bpftb->hier_block2::connect(bpfsrc, 0, bpffir, 0);
	bpftb->hier_block2::connect(bpfhead, 0, bpffile, 0);
	bpftb->hier_block2::connect(bpffir, 0, bpfhead, 0);
	bpftb->start();
	bpftb->wait();
}

void test_filter(const char *name, window::win_type type, vector<float> &in, int decimation, float gain, float samplerate, float cutoff, float transition_width)
{
	auto lpftaps = firdes::low_pass(gain, samplerate, cutoff, transition_width, (firdes::win_type)type);
	auto hpftaps = firdes::high_pass(gain, samplerate, cutoff, transition_width, (firdes::win_type)type);
	assert(lpftaps.size() == hpftaps.size());

	gr::filter::kernel::fir_filter_fff lpf(decimation, lpftaps);
	gr::filter::kernel::fir_filter_fff hpf(decimation, hpftaps);

	auto taplen = lpftaps.size();
	auto inplen = in.size();

	vector<float> lpfout, hpfout;
	lpfout.resize(inplen);
	hpfout.resize(inplen);

	// applying a convolution to the input is a window of tap length size sliding to the left
	// we start with 0s input as there is no signal, as we slide the signal left
	// the pad length added acts as a delay
	auto buf = pad_array_f(in, 0, taplen, false);
	for (auto i = 0u; i < inplen; i++)
	{
		lpfout[i] = lpf.filter(&buf[i + 1]);
		hpfout[i] = hpf.filter(&buf[i + 1]);
	}

	printf("%s\n", name);
	printf("Decimation %d\n", decimation);
	printf("Gain %f\n", gain);
	printf("Sample Rate %f\n", samplerate);
	printf("Cutoff Frequency %f\n", cutoff);
	printf("Transition Width %f\n", transition_width);
	printf("Tap Size %zu\n", taplen);

	for (auto i = 0u; i < inplen; i++)
	{
		printf("%u %.6f %.6f\n", i, lpfout[i], hpfout[i]);
	}

	printf("\n");
}

int main()
{
	srand48(time(NULL));
	for (auto f : filters)
	{
		auto in = constant_source(1, 32);
		test_topblock(f.name, f.type, in, 1, 1, 32, 8, 16, 4);
		test_filter(f.name, f.type, in, 1, 1, 32, 8, 4);
	}
	return 0;
}
