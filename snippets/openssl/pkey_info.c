#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <err.h>
#include <openssl/pem.h>

void
usage(void)
{
	fprintf(stderr, "usage: [keyfile]\n");
	fprintf(stderr, "\t-h\tshow this message\n");
	exit(2);
}

BIO *
newbio(const BIO_METHOD *type)
{
	BIO *bio;

	bio = BIO_new(type);
	if (!bio)
		errx(1, "failed to make BIO");
	return bio;
}

int
main(int argc, char *argv[])
{
	BIO *bio;
	EVP_PKEY *key;
	char mdname[128];
	char buf[1];
	int mdnid;
	int c;

	while ((c = getopt(argc, argv, "h")) != -1) {
		switch (c) {
		case 'h':
			usage();
			break;
		}
	}
	argc -= optind;
	argv += optind;

	if (argc < 1) {
		bio = newbio(BIO_s_mem());
		while ((c = fgetc(stdin)) != EOF) {
			buf[0] = c;
			BIO_write(bio, buf, 1);
		}
	} else {
		bio = BIO_new(BIO_s_file());
		if (!BIO_read_filename(bio, argv[0]))
			errx(1, "failed to open key file");
	}

	key = PEM_read_bio_PrivateKey(bio, NULL, NULL, NULL);
	if (!key)
		errx(1, "failed to read private key");

	mdname[0] = '\0';
	mdnid = 0;
	EVP_PKEY_get_default_digest_name(key, mdname, sizeof(mdname));
	EVP_PKEY_get_default_digest_nid(key, &mdnid);

	printf("ID: %d\n", EVP_PKEY_id(key));
	printf("Base ID: %d\n", EVP_PKEY_base_id(key));
	printf("Security bits: %d\n", EVP_PKEY_security_bits(key));
	printf("Bits: %d\n", EVP_PKEY_bits(key));
	printf("Size: %d\n", EVP_PKEY_size(key));
	printf("Can sign: %d\n", EVP_PKEY_can_sign(key));
	printf("Description: %s\n", EVP_PKEY_get0_description(key));
	printf("Digest NID: %d\n", mdnid);
	printf("Digest name: %s\n", mdname);

	BIO_free(bio);
	EVP_PKEY_free(key);
	return 0;
}
