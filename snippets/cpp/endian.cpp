#include <cstdio>
#include <cstdint>
#include <cinttypes>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

template <typename T>
T leget(void *b)
{
	T v = 0;

	auto p = (uint8_t *)b;
	for (ssize_t i = sizeof(T) - 1; i >= 0; i--)
		v = (v << 8) | p[i];
	return v;
}

template <typename T>
T beget(void *b)
{
	T v = 0;

	auto p = (uint8_t *)b;
	for (size_t i = 0; i < sizeof(T); i++)
		v = (v << 8) | p[i];
	return v;
}

const auto beget2 = beget<uint16_t>;
const auto beget4 = beget<uint32_t>;
const auto beget8 = beget<uint64_t>;

const auto leget2 = leget<uint16_t>;
const auto leget4 = leget<uint32_t>;
const auto leget8 = leget<uint64_t>;

void test(uint8_t *b, size_t n)
{
	if (n >= 2)
		printf("get2 be %jX le %jX\n", (uintmax_t)beget2(b), (uintmax_t)leget2(b));

	if (n >= 4)
		printf("get4 be %jX le %jX\n", (uintmax_t)beget4(b), (uintmax_t)leget4(b));

	if (n >= 8)
		printf("get8 be %jX le %jX\n", (uintmax_t)beget8(b), (uintmax_t)leget8(b));

	printf("\n");
}

int main()
{
	uint8_t a[] = { 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8 };
	uint8_t b[] = { 0x3, 0x0 };
	uint8_t c[] = { 0xde, 0xad, 0xbe, 0xef };

	test(a, nelem(a));
	test(b, nelem(b));
	test(c, nelem(c));

	return 0;
}
