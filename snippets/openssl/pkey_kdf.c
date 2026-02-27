#include <stdio.h>
#include <openssl/evp.h>
#include <openssl/kdf.h>

int
test_kdf_hkdf(void)
{
	int ret = 0;
	EVP_PKEY_CTX *pctx;
	unsigned char out[80];
	size_t i, outlen;

	outlen = sizeof(out);
	ret = 0;
	if ((pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_HKDF, NULL)) == NULL) {
		goto err;
	}
	if (EVP_PKEY_derive_init(pctx) <= 0) {
		goto err;
	}
	if (EVP_PKEY_CTX_set_hkdf_md(pctx, EVP_sha512()) <= 0) {
		goto err;
	}
	if (EVP_PKEY_CTX_set1_hkdf_salt(pctx, (const unsigned char *)"salt", 4) <= 0) {
		goto err;
	}
	if (EVP_PKEY_CTX_set1_hkdf_key(pctx, (const unsigned char *)"secret\n", 7) <= 0) {
		goto err;
	}
	if (EVP_PKEY_CTX_add1_hkdf_info(pctx, (const unsigned char *)"label", 5) <= 0) {
		goto err;
	}
	if (EVP_PKEY_derive(pctx, out, &outlen) <= 0) {
		goto err;
	}

	for (i = 0; i < outlen; i++)
		printf("%02x ", out[i]);
	printf("\n");

	ret = 1;

err:
	EVP_PKEY_CTX_free(pctx);
	return ret;
}

int
main(void)
{
	test_kdf_hkdf();
	return 0;
}
