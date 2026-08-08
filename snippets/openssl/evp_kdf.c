#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <err.h>
#include <openssl/evp.h>
#include <openssl/kdf.h>
#include <openssl/params.h>

int
main(int argc, char *argv[])
{
	EVP_KDF *kdf;
	EVP_KDF_CTX *kctx = NULL;
	unsigned char secret[128], derived[128];
	OSSL_PARAM params[5], *p = params;

	if ((kdf = EVP_KDF_fetch(NULL, "hkdf", NULL)) == NULL) {
		errx(1, "EVP_KDF_fetch");
	}
	kctx = EVP_KDF_CTX_new(kdf);

	EVP_KDF_free(kdf); 
	if (kctx == NULL) {
		errx(1, "EVP_KDF_CTX_new");
	}

	memcpy(secret, "secret", 6);
	*p++ = OSSL_PARAM_construct_utf8_string("digest", "sha512", 7);
	*p++ = OSSL_PARAM_construct_octet_string("salt", "salt", 4);
	*p++ = OSSL_PARAM_construct_octet_string("key", secret, 6);
	*p++ = OSSL_PARAM_construct_octet_string("info", "label", 5);
	*p = OSSL_PARAM_construct_end();
	if (EVP_KDF_CTX_set_params(kctx, params) <= 0) {
		errx(1, "EVP_KDF_CTX_set_params");
	}

	if (EVP_KDF_derive(kctx, derived, sizeof(derived), NULL) <= 0) {
		errx(1, "EVP_KDF_derive");
	}

	const unsigned char *key = derived + 0;
	const unsigned char *iv = derived + 32;

	printf("Key: ");
	for (size_t i = 0; i < 32; ++i)
		printf("%02x ", key[i]);
	printf("\n");

	printf("IV:  ");
	for (size_t i = 0; i < 16; ++i)
		printf("%02x ", iv[i]);
	printf("\n");

	EVP_KDF_CTX_free(kctx);

	return 0;
}
