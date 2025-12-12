#include <assert.h>
#include <string.h>
#include <getopt.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

void
printhex(void *buffer, size_t size)
{
	unsigned char *ptr;

	ptr = buffer;
	for (size_t i = 0; i < size; i++)
		printf("%02x", ptr[i]);
	printf("\n");
}

int
main(int argc, char *argv[])
{
	EVP_CIPHER_CTX *ctx;

	unsigned char *in;
	unsigned char *out;
	unsigned char *key;
	unsigned char *iv;
	size_t inlen;
	size_t keylen;
	int outlen;
	int r;
	int c;
	const EVP_CIPHER *(*evpfn)(void);

	evpfn = EVP_aes_128_cfb128;
	while ((c = getopt(argc, argv, "m") != -1)) {
		switch (c) {
			case 'm':
				if (!strcmp(optarg, "ecb"))
					evpfn = EVP_aes_128_ecb;
				else if (!strcmp(optarg, "cfb"))
					evpfn = EVP_aes_128_cfb128;
				break;
		}
	}
	argc -= optind;
	argv += optind;

	if (argc != 3) {
		printf("usage: <key> <in> <out>\n");
		exit(2);
	}

	r = g_file_get_contents(argv[0], (char **)&key, &keylen, NULL);
	assert(r == TRUE);

	iv = g_malloc0(keylen * sizeof(*iv));
	assert(iv);

	r = g_file_get_contents(argv[1], (char **)&in, &inlen, NULL); 
	assert(r == TRUE);

	out = g_malloc0(inlen);
	assert(out);

	printf("in length: %zu\n", inlen);
	printf("key length: %zu\n", keylen);
	printhex(key, keylen);

	ctx = EVP_CIPHER_CTX_new();
	assert(ctx);

	r = EVP_DecryptInit_ex(ctx, evpfn(), NULL, key, iv);
	assert(r == 1);

	r = EVP_CIPHER_CTX_set_padding(ctx, 0);
	assert(r == 1);

	r = EVP_DecryptUpdate(ctx, out, &outlen, in, inlen);
	assert(r == 1);

	printf("out length: %d\n", outlen);
	r = g_file_set_contents(argv[2], (char *)out, outlen, NULL);
	assert(r == TRUE);

	EVP_CIPHER_CTX_free(ctx);
	g_free(key);
	g_free(in);
	g_free(out);
	g_free(iv);

	return 0;
}
