#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

unsigned long
pjwhash(unsigned char *str)
{
	unsigned long g, h;

	for (h = 0; *str != 0; str++) {
		h = (h << 4) + (*str & 0xFF);
		g = h & 0xF0000000;
		h ^= (g >> 24) | g;
	}
	return h;
}

unsigned long
sdbmhash(unsigned char *str)
{
	unsigned long hash = 0;
	int c;

	while ((c = *str++))
		hash = c + (hash << 6) + (hash << 16) - hash;

	return hash;
}

unsigned long
djbhash(unsigned char *str)
{
	unsigned long hash = 5381;
	int c;

	while ((c = *str++))
		hash = ((hash << 5) + hash) + c;

	return hash;
}

unsigned long
krhash(unsigned char *str)
{
	unsigned int hash = 0;
	int c;

	while ((c = *str++))
		hash += c;

	return hash;
}

#define FNV_32_PRIME (0x01000193UL)

unsigned long
fnv1hash(unsigned char *str)
{
	unsigned long hash = 0;

	while (*str) {
		hash *= FNV_32_PRIME;
		hash ^= *str++;
	}
	return hash;
}

int
shufflecmp(const void *a, const void *b)
{
	(void)a;
	(void)b;
	return ((rand() >> 10) % 1023) - 511;
}

void
shuffle(unsigned char *p, size_t n)
{
	size_t i;

	for (i = 0; i < n; i++)
		p[i] = i;
	qsort(p, n, sizeof(*p), shufflecmp);
}

unsigned long
pearson64(unsigned char *str)
{
	static unsigned char tab[256];
	static unsigned char init;
	unsigned char h, hh[8];
	unsigned long v;
	size_t i, j, len;

	if (!init) {
		shuffle(tab, nelem(tab));
		init = 1;
	}

	len = strlen((char *)str);

	for (i = 0; i < 8; i++) {
		h = tab[(str[0] + i) % 256];
		for (j = 1; j < len; j++)
			h = tab[h ^ str[j]];
		hh[i] = h;
	}

	v = 0;
	for (i = 0; i < 8; i++)
		v |= ((unsigned long)hh[i]) << (8 * i);
	return v;
}

#define mix(a, b, c)               \
	{                          \
		a = a - b;         \
		a = a - c;         \
		a = a ^ (c >> 13); \
		b = b - c;         \
		b = b - a;         \
		b = b ^ (a << 8);  \
		c = c - a;         \
		c = c - b;         \
		c = c ^ (b >> 13); \
		a = a - b;         \
		a = a - c;         \
		a = a ^ (c >> 12); \
		b = b - c;         \
		b = b - a;         \
		b = b ^ (a << 16); \
		c = c - a;         \
		c = c - b;         \
		c = c ^ (b >> 5);  \
		a = a - b;         \
		a = a - c;         \
		a = a ^ (c >> 3);  \
		b = b - c;         \
		b = b - a;         \
		b = b ^ (a << 10); \
		c = c - a;         \
		c = c - b;         \
		c = c ^ (b >> 15); \
	}

unsigned long
newhash32(unsigned char *str)
{
	size_t n, len;
	unsigned long a, b, c;

	n = len = strlen((char *)str);
	a = 0x9e3779b9;
	b = 0x9e3779b9;
	c = str[0];

	while (len >= 12) {
		a = a + (str[0] + ((unsigned long)str[1] << 8) + ((unsigned long)str[2] << 16) + ((unsigned long)str[3] << 24));
		b = b + (str[4] + ((unsigned long)str[5] << 8) + ((unsigned long)str[6] << 16) + ((unsigned long)str[7] << 24));
		c = c + (str[8] + ((unsigned long)str[9] << 8) + ((unsigned long)str[10] << 16) + ((unsigned long)str[11] << 24));
		mix(a, b, c);
		str = str + 12;
		len = len - 12;
	}

	c += n;
	switch (len) {
	case 11:
		c = c + ((unsigned long)str[10] << 24);
		// fallthrough
	case 10:
		c = c + ((unsigned long)str[9] << 16);
		// fallthrough
	case 9:
		c = c + ((unsigned long)str[8] << 8);
		// fallthrough
	case 8:
		b = b + ((unsigned long)str[7] << 24);
		// fallthrough
	case 7:
		b = b + ((unsigned long)str[6] << 16);
		// fallthrough
	case 6:
		b = b + ((unsigned long)str[5] << 8);
		// fallthrough
	case 5:
		b = b + str[4];
		// fallthrough
	case 4:
		a = a + ((unsigned long)str[3] << 24);
		// fallthrough
	case 3:
		a = a + ((unsigned long)str[2] << 16);
		// fallthrough
	case 2:
		a = a + ((unsigned long)str[1] << 8);
		// fallthrough
	case 1:
		a = a + str[0];
	}
	mix(a, b, c);

	return c;
}

#undef mix

uint32_t
wanghash32(uint32_t key)
{
	key += ~(key << 15);
	key ^= (key >> 10);
	key += (key << 3);
	key ^= (key >> 6);
	key += ~(key << 11);
	key ^= (key >> 16);
	return key;
}

uint32_t
unwanghash32(uint32_t hashval)
{
	uint32_t k6 = hashval;
	uint32_t k5 = k6 ^ (k6 >> 16);
	uint32_t k4 = 4290770943 * ~k5;
	uint32_t k3 = k4 ^ (k4 >> 6) ^ (k4 >> 12) ^ (k4 >> 18) ^ (k4 >> 24) ^ (k4 >> 30);
	uint32_t k2 = 954437177 * k3;
	uint32_t k1 = k2 ^ (k2 >> 10) ^ (k2 >> 20) ^ (k2 >> 30);
	uint32_t k0 = 3221192703 * ~k1;
	return k0;
}

void
strhash(void)
{
	unsigned char str[] = "hello world!\n";
	printf("%lx %lx %lx %lx %lx %lx %lx\n",
	       pjwhash(str), sdbmhash(str), djbhash(str), krhash(str), fnv1hash(str), pearson64(str), newhash32(str));
}

void
inthash(void)
{
	int i;
	for (i = 0; i < 1024; i++)
		printf("%ju %ju\n", (uintmax_t)unwanghash32(wanghash32(i)), (uintmax_t)wanghash32(i) & 0xfff);

	for (i = 1000; i <= 10000; i++)
		printf("%ju %ju\n", (uintmax_t)unwanghash32(wanghash32(i)), (uintmax_t)wanghash32(i) & 0xffff);
}

int
main(void)
{
	srand(time(NULL));
	strhash();
	inthash();
	return 0;
}
