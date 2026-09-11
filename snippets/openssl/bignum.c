#include <stdio.h>
#include <string.h>
#include <openssl/bn.h>

int
test_bn_basic(int bits)
{
	BIGNUM *x, *y;
	int r;

	r = 0;
	x = BN_new();
	y = BN_new();
	if (!x || !y) {
		printf("failed to allocate numbers\n");
		goto error;
	}

	if (!BN_rand(x, bits, 0, 0) || !BN_pseudo_rand(y, bits, 0, 0)) {
		printf("failed to generate random number\n");
		goto error;
	}

	BN_print_fp(stdout, x);
	printf("\n");
	BN_print_fp(stdout, y);
	printf("\n\n");

	if (0) {
	error:
		r = -1;
	}

	BN_free(x);
	BN_free(y);

	return r;
}

int
test_bn_bin(void)
{
	unsigned char buf[256];
	BIGNUM *x;
	int r;

	r = 0;
	x = BN_new();
	if (!x) {
		printf("failed to allocate number\n");
		goto error;
	}

	memset(buf, 0x55, sizeof(buf));
	if (!BN_bin2bn(buf, sizeof(buf), x)) {
		printf("failed to serialize buffer to number\n");
		goto error;
	}

	BN_print_fp(stdout, x);
	printf("\n");

	if (0) {
	error:
		r = -1;
	}

	BN_free(x);
	return r;
}

int
main(void)
{
	int i;

	for (i = 1; i <= 10; i++)
		test_bn_basic(1 << i);
	test_bn_bin();

	return 0;
}
