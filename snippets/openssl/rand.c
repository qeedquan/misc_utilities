#include <stdio.h>
#include <string.h>
#include <time.h>
#include <openssl/rand.h>

void
test_engine(void)
{
	// By default, OpenSSL uses rand_ssleay_meth 
	printf("OpenSSL Method = %p\n", (void *)RAND_OpenSSL());
	printf("Method = %p\n", (void *)RAND_get_rand_method());
}

int
test_rand(size_t size)
{
	unsigned char *buf;
	time_t t;
	int r;

	printf("Rand: %zu\n", size);
	r = 0;
	t = time(NULL);
	RAND_add(&t, sizeof(t), 0);

	buf = OPENSSL_malloc(size);
	if (!buf) {
		printf("failed to allocate memory for random buffer\n");
		goto error;
	}

	// RAND can fail when not enough entropy is added
	if (RAND_pseudo_bytes(buf, size) < 0) {
		printf("failed to generate pseudo bytes of %zu bytes\n", size);
		r = -1;
	}

	if (RAND_bytes(buf, size) <= 0) {
		printf("failed to generate random bytes of %zu bytes\n", size);
		r = -1;
	}

	if (0) {
	error:
		r = -1;
	}

	if (buf)
		OPENSSL_cleanse(buf, size);

	OPENSSL_free(buf);

	return r;
}

int
main(void)
{
	int i;

	test_engine();
	for (i = 1; i <= 12; i++)
		test_rand(1 << i);
	return 0;
}
