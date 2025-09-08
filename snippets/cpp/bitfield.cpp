#include <cstdio>
#include <cstdint>
#include <cinttypes>

template <int N>
struct bitfield
{
	int a : N;
	int b : N + 1;
	uint64_t c : 8 * N;
	uint8_t d : 1;
};

int main()
{
	bitfield<8> b;
	bitfield<4> a;
	bitfield<2> c;
	bitfield<1> d;
	b.c = 20;
	a.b = 3;
	c.a = -2;
	d.d = 1;
	printf("%ju %d %d %d\n", (uintmax_t)b.c, a.b, c.a, d.d);
	return 0;
}
