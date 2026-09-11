#include <assert.h>
#include <stdio.h>
#include <smmintrin.h>
#include <immintrin.h>

void
load_store()
{
	// load 8 32 bit integers into a 256 bit register
	__m256i values = _mm256_setr_epi32(0x1234, 0x2345, 0x3456, 0x4567, 0x5678, 0x6789, 0x789A, 0x89AB);

	// extract the integer out
	// the intrinsic only accepts constant indices that is resolved at compile tile
	int x0 = _mm256_extract_epi32(values, 0);
	int x1 = _mm256_extract_epi32(values, 1);
	int x2 = _mm256_extract_epi32(values, 2);
	int x3 = _mm256_extract_epi32(values, 3);
	int x4 = _mm256_extract_epi32(values, 4);
	int x5 = _mm256_extract_epi32(values, 5);
	int x6 = _mm256_extract_epi32(values, 6);
	int x7 = _mm256_extract_epi32(values, 7);
	printf("%#x %#x %#x %#x %#x %#x %#x %#x\n", x0, x1, x2, x3, x4, x5, x6, x7);

	// load 8 32 bit integers specified in an array into a 256 bit register
	int array[8] = { 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80 };
	int xarray[8] = {};
	values = _mm256_loadu_si256((__m256i *)array);
	// extract it out
	x0 = _mm256_extract_epi32(values, 0);
	x1 = _mm256_extract_epi32(values, 1);
	x2 = _mm256_extract_epi32(values, 2);
	x3 = _mm256_extract_epi32(values, 3);
	x4 = _mm256_extract_epi32(values, 4);
	x5 = _mm256_extract_epi32(values, 5);
	x6 = _mm256_extract_epi32(values, 6);
	x7 = _mm256_extract_epi32(values, 7);
	printf("%#x %#x %#x %#x %#x %#x %#x %#x\n", x0, x1, x2, x3, x4, x5, x6, x7);
	// another way to extract it to an array
	_mm256_storeu_si256((__m256i *)xarray, values);
	printf("%#x %#x %#x %#x %#x %#x %#x %#x\n", xarray[0], xarray[1], xarray[2], xarray[3], xarray[4], xarray[5], xarray[6], xarray[7]);
}

void
arithmetic()
{
	// load constants into 2 256 bit register
	__m256i first_values = _mm256_setr_epi32(10, 20, 30, 40, 50, 60, 70, 80);
	__m256i second_values = _mm256_setr_epi32(1, 2, 3, 4, 5, 6, 7, 8);
	// add the 2 256 bit register together 32 bits unit
	__m256i result_values = _mm256_add_epi32(first_values, second_values);
	// extract the values out
	int x0 = _mm256_extract_epi32(result_values, 0);
	int x1 = _mm256_extract_epi32(result_values, 1);
	int x2 = _mm256_extract_epi32(result_values, 2);
	int x3 = _mm256_extract_epi32(result_values, 3);
	int x4 = _mm256_extract_epi32(result_values, 4);
	int x5 = _mm256_extract_epi32(result_values, 5);
	int x6 = _mm256_extract_epi32(result_values, 6);
	int x7 = _mm256_extract_epi32(result_values, 7);
	printf("%d %d %d %d %d %d %d %d\n", x0, x1, x2, x3, x4, x5, x6, x7);
}

int
main()
{
	assert(__builtin_cpu_supports("sse4.2"));
	load_store();
	arithmetic();
	return 0;
}
