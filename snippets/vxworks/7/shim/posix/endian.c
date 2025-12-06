#include "endian.h"

uint16_t
be16dec(const void *b)
{
	const uint8_t *p;
	uint16_t v;

	p = b;
	v = (uint16_t)p[1];
	v |= (uint16_t)p[0] << 8;

	return v;
}

uint32_t
be32dec(const void *b)
{
	const uint8_t *p;
	uint32_t v;

	p = b;
	v = (uint32_t)p[3];
	v |= (uint32_t)p[2] << 8;
	v |= (uint32_t)p[1] << 16;
	v |= (uint32_t)p[0] << 24;

	return v;
}

uint64_t
be64dec(const void *b)
{
	const uint8_t *p;
	uint64_t v;

	p = b;
	v = (uint64_t)p[7];
	v |= (uint64_t)p[6] << 8;
	v |= (uint64_t)p[5] << 16;
	v |= (uint64_t)p[4] << 24;
	v |= (uint64_t)p[3] << 32;
	v |= (uint64_t)p[2] << 40;
	v |= (uint64_t)p[1] << 48;
	v |= (uint64_t)p[0] << 56;

	return v;
}

uint16_t
le16dec(const void *b)
{
	const uint8_t *p;
	uint16_t v;

	p = b;
	v = (uint16_t)p[0];
	v |= (uint16_t)p[1] << 8;

	return v;
}

uint32_t
le32dec(const void *b)
{
	const uint8_t *p;
	uint32_t v;

	p = b;
	v = (uint32_t)p[0];
	v |= (uint32_t)p[1] << 8;
	v |= (uint32_t)p[2] << 16;
	v |= (uint32_t)p[3] << 24;

	return v;
}

uint64_t
le64dec(const void *b)
{
	const uint8_t *p;
	uint64_t v;

	p = b;
	v = (uint64_t)p[0];
	v |= (uint64_t)p[1] << 8;
	v |= (uint64_t)p[2] << 16;
	v |= (uint64_t)p[3] << 24;
	v |= (uint64_t)p[4] << 32;
	v |= (uint64_t)p[5] << 40;
	v |= (uint64_t)p[6] << 48;
	v |= (uint64_t)p[7] << 56;

	return v;
}

void
be16enc(void *b, uint16_t v)
{
	uint8_t *p;

	p = b;
	p[1] = (v >> 0) & 0xff;
	p[0] = (v >> 8) & 0xff;
}

void
be32enc(void *b, uint32_t v)
{
	uint8_t *p;

	p = b;
	p[3] = (v >> 0) & 0xff;
	p[2] = (v >> 8) & 0xff;
	p[1] = (v >> 16) & 0xff;
	p[0] = (v >> 24) & 0xff;
}

void
be64enc(void *b, uint64_t v)
{
	uint8_t *p;

	p = b;
	p[7] = (v >> 0) & 0xff;
	p[6] = (v >> 8) & 0xff;
	p[5] = (v >> 16) & 0xff;
	p[4] = (v >> 24) & 0xff;
	p[3] = (v >> 32) & 0xff;
	p[2] = (v >> 40) & 0xff;
	p[1] = (v >> 48) & 0xff;
	p[0] = (v >> 56) & 0xff;
}

void
le16enc(void *b, uint16_t v)
{
	uint8_t *p;

	p = b;
	p[0] = (v >> 0) & 0xff;
	p[1] = (v >> 8) & 0xff;
}

void
le32enc(void *b, uint32_t v)
{
	uint8_t *p;

	p = b;
	p[0] = (v >> 0) & 0xff;
	p[1] = (v >> 8) & 0xff;
	p[2] = (v >> 16) & 0xff;
	p[3] = (v >> 24) & 0xff;
}

void
le64enc(void *b, uint64_t v)
{
	uint8_t *p;

	p = b;
	p[0] = (v >> 0) & 0xff;
	p[1] = (v >> 8) & 0xff;
	p[2] = (v >> 16) & 0xff;
	p[3] = (v >> 24) & 0xff;
	p[4] = (v >> 32) & 0xff;
	p[5] = (v >> 40) & 0xff;
	p[6] = (v >> 48) & 0xff;
	p[7] = (v >> 56) & 0xff;
}

uint16_t
bswap16(uint16_t x)
{
	return ((x & 0xff00ull) >> 8) |
	       ((x & 0x00ffull) << 8);
}

uint32_t
bswap32(uint32_t x)
{
	return (((x & 0xff000000ull)) >> 24) |
	       (((x & 0x00ff0000ull)) >> 8) |
	       (((x & 0x000000ffull)) << 24) |
	       (((x & 0x0000ff00ull)) << 8);
}

uint64_t
bswap64(uint64_t x)
{
	return ((((x) & 0xff00000000000000ull) >> 56) |
	        (((x) & 0x00ff000000000000ull) >> 40) |
	        (((x) & 0x0000ff0000000000ull) >> 24) |
	        (((x) & 0x000000ff00000000ull) >> 8) |
	        (((x) & 0x00000000ff000000ull) << 8) |
	        (((x) & 0x0000000000ff0000ull) << 24) |
	        (((x) & 0x000000000000ff00ull) << 40) |
	        (((x) & 0x00000000000000ffull) << 56));
}
