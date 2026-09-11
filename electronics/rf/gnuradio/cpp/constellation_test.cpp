#include "common.h"

void test_symbol_mapping(const char *name, constellation_sptr constellation)
{
	printf("Testing symbol mapping on %s\n\n", name);
	printf("Dimensionality       %u\n", constellation->dimensionality());
	printf("Bits per symbol      %u\n", constellation->bits_per_symbol());
	printf("Rotational Symmetry  %u\n", constellation->rotational_symmetry());
	printf("Arity                %u\n", constellation->arity());

	// the symbols are basically indices to the point table
	// if we use symbols outside of the range of the constellation mapping table
	// it will overread the table, can get this to buffer overflow if use too large of a value index
	printf("\n");
	printf("Symbol <-> Point mapping\n");
	auto bps = constellation->bits_per_symbol();
	for (auto i = 0u; i < (1u << bps); i++)
	{
		auto points = constellation->map_to_points_v(i);
		for (auto p : points)
		{
			auto value = constellation->decision_maker(&p);
			printf("%u | (% .6f%+.6fi) | %u\n", i, p.real(), p.imag(), value);
		}
	}

	printf("--------------------------\n\n");
}

int main()
{
	test_symbol_mapping("BPSK", constellation_bpsk::make());
	test_symbol_mapping("QPSK", constellation_qpsk::make());
	test_symbol_mapping("8PSK", constellation_8psk::make());
	test_symbol_mapping("16QAM", constellation_16qam::make());

	return 0;
}
