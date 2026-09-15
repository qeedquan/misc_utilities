// given a 2's complement value of a bit width less than a type we store it into
// we want to sign extend it to the larger type so we can represent the smaller
// type to a larger type

// example: a sensor register is 4 bits wide and the sign bit is in the leftmost bit
// (svvv) <- 4 bit register so the range is [-8, 7]
// we read it into a uint8, but want to preserve the sign in int8
#include <cstdio>
#include <cstdint>
#include <climits>
#include <cassert>

template <typename T, typename U>
T sign_ext_read_1(U measured, size_t size)
{
	assert(size > 0 && size < sizeof(U) * CHAR_BIT);
	assert(sizeof(T) >= sizeof(U));

	size_t mask = 1ULL << (size - 1);
	return (measured & (mask - 1)) - (measured & mask);
}

template <typename T, typename U>
T sign_ext_read_2(U measured, size_t size)
{
	assert(size > 0 && size < sizeof(U) * CHAR_BIT);
	assert(sizeof(T) >= sizeof(U));

	size_t bits = sizeof(T) * CHAR_BIT;
	size_t shift = bits - size;
	T val = measured;
	val <<= shift;
	val >>= shift;
	return val;
}

template <typename T, typename U>
void test(size_t bits)
{
	printf("T: %zu U: %zu bits: %zu\n", sizeof(T) * CHAR_BIT, sizeof(U) * CHAR_BIT, bits);
	for (U i = 0; i < (1ULL << bits); i++)
	{
		T x = sign_ext_read_1<T, U>(i, bits);
		T y = sign_ext_read_2<T, U>(i, bits);
		assert(x == y);
		printf("%jd\n", (intmax_t)x);
	}
}

int main()
{
	test<int32_t, uint8_t>(1);
	test<int32_t, uint8_t>(2);
	test<int32_t, uint8_t>(3);
	test<int32_t, uint8_t>(4);
	test<int8_t, uint8_t>(4);
	test<int32_t, uint16_t>(13);
	test<int64_t, uint32_t>(31);
	return 0;
}
