#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

typedef struct {
	uint8_t state[256];
	uint8_t x, y;
} RC4;

static void
swapb(uint8_t *x, uint8_t *y)
{
	uint8_t t;

	t = *x;
	*x = *y;
	*y = t;
}

void
initrc4(RC4 *r, uint8_t *data, size_t len)
{
	uint8_t *s, i1, i2;
	int i;

	s = r->state;
	for (i = 0; i < 256; i++)
		s[i] = i;

	r->x = 0;
	r->y = 0;
	i1 = 0;
	i2 = 0;

	for (i = 0; i < 256; i++) {
		i2 = (data[i1] + s[i] + i2) % 256;
		swapb(&s[i], &s[i2]);
		i1 = (i1 + 1) % len;
	}
}

void
rc4enc(RC4 *r, uint8_t *buf, size_t len)
{
	uint8_t *s, x, y, xi;
	size_t i;

	x = r->x;
	y = r->y;

	s = r->state;
	for (i = 0; i < len; i++) {
		x = (x + 1) % 256;
		y = (s[x] + y) % 256;
		swapb(&s[x], &s[y]);

		xi = s[x] + (s[y] % 256);
		buf[i] ^= s[xi];
	}
	r->x = x;
	r->y = y;
}

int
main(void)
{
	RC4 rc4[1];
	uint8_t seed[256], buf[BUFSIZ];
	size_t i, nr;

	srand(time(NULL));
	for (i = 0; i < sizeof(seed); i++)
		seed[i] = rand() & 0xff;
	initrc4(rc4, seed, sizeof(seed));

	for (;;) {
		if ((nr = fread(buf, 1, BUFSIZ, stdin)) == 0)
			break;
		rc4enc(rc4, buf, nr);
		fwrite(buf, nr, 1, stdout);
	}

	return 0;
}
