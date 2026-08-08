#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <glib.h>
#include <openssl/evp.h>
#include <openssl/sha.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

typedef unsigned char *(*hashfn)(const unsigned char *, size_t, unsigned char *);

typedef struct {
	char name[32];
	hashfn func;
	size_t len;
} Hash;

const Hash *
gethash(const char *name)
{
	static const Hash hashes[] = {
	    {"sha1", SHA1, SHA_DIGEST_LENGTH},
	    {"sha224", SHA224, SHA224_DIGEST_LENGTH},
	    {"sha256", SHA256, SHA256_DIGEST_LENGTH},
	    {"sha384", SHA384, SHA384_DIGEST_LENGTH},
	    {"sha512", SHA512, SHA512_DIGEST_LENGTH},
	};

	const Hash *h;
	size_t i;

	for (i = 0; i < nelem(hashes); i++) {
		h = hashes + i;
		if (!strcmp(name, h->name))
			return h;
	}
	return NULL;
}

void
usage(void)
{
	fprintf(stderr, "usage: <algorithm> <file>\n");
	fprintf(stderr, "supported algorithms: sha1 sha224 sha256 sha384 sha512\n");
	exit(2);
}

int
main(int argc, char *argv[])
{
	const Hash *h;
	unsigned char *b, md[EVP_MAX_MD_SIZE];
	size_t i, n;

	if (argc != 3)
		usage();

	h = gethash(argv[1]);
	if (!h)
		errx(1, "unsupported hash function");

	if (!g_file_get_contents(argv[2], (void *)&b, &n, NULL))
		errx(1, "failed to read file");

	h->func(b, n, md);
	for (i = 0; i < h->len; i++)
		printf("%02x", md[i]);
	printf("\n");

	g_free(b);
	return 0;
}
