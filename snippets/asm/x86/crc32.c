// cc -o crc32 crc32.c -msse4.2
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <smmintrin.h>

typedef uint64_t __attribute__((__may_alias__)) u64a;

uint32_t
crc32(const uint8_t *buf, size_t len)
{
	uint32_t hash;

	hash = 0;
	while (len > 8) {
		hash = _mm_crc32_u64(hash, *((u64a *)buf));
		buf += 8;
		len -= 8;
	}

	while (len > 0) {
		hash = _mm_crc32_u8(hash, buf[0]);
		buf += 1;
		len -= 1;
	}
	return hash;
}

int
main(void)
{
	const char str[] = "hello world and such";
	uint32_t hash;

	__builtin_cpu_init();
	assert(__builtin_cpu_supports("sse4.2"));
	hash = crc32(str, sizeof(str) - 1);
	printf("%x\n", hash);

	return 0;
}
