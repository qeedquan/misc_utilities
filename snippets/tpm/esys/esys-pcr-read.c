// read pcr values

#include "common.h"

int
get_pcr_values(ESYS_CONTEXT *ctx, TPM2_ALG_ID alg, TPML_DIGEST **pcrvals)
{

	TPML_PCR_SELECTION pcrsel;
	TPML_PCR_SELECTION *pcrout;
	UINT32 pcrcnt;
	int i, r;

	memset(&pcrsel, 0, sizeof(pcrsel));
	pcrsel.count = 1;
	pcrsel.pcrSelections[0].hash = alg;

	// size of bit vector on what we want to read
	pcrsel.pcrSelections[0].sizeofSelect = 3;
	for (i = 0; i <= 23; i++) {
		// bit vector of pcr we want to read
		pcrsel.pcrSelections[0].pcrSelect[i / 8] |= 1 << (i % 8);

		r = Esys_PCR_Read(ctx, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, &pcrsel, &pcrcnt, &pcrout, &pcrvals[i]);
		if (r != TSS2_RC_SUCCESS)
			return -1;

		pcrsel.pcrSelections[0].pcrSelect[0] = 0;
		pcrsel.pcrSelections[0].pcrSelect[1] = 0;
		pcrsel.pcrSelections[0].pcrSelect[2] = 0;

		Esys_Free(pcrout);
	}
	return i;
}

void
gen_c_array(const char *id, TPML_DIGEST **pcrvals, size_t pcrlen)
{
	size_t i, j;

	printf("const unsigned char %s_pcr_values[] = {\n", id);
	for (i = 0; i < pcrlen; i++) {
		printf("\t// %zu (%d bytes)\n", i, pcrvals[i]->digests[0].size);
		printf("\t");
		for (j = 0; j < pcrvals[i]->digests[0].size; j++) {
			printf("0x%02x, ", pcrvals[i]->digests[0].buffer[j]);
			if ((j & 7) == 7)
				printf("\n\t");
		}
		printf("\n\n");
	}
	printf("};\n");
	printf("\n");
}

int
pcr_compare_values(TPML_DIGEST **pcrvals, int selections[], int selection_length, unsigned char *digests, size_t digest_size)
{
	int i;

	for (i = 0; i < selection_length; i++) {
		if (pcrvals[selections[i]]->digests[0].size != digest_size)
			return -ERANGE;

		if (memcmp(digests + (digest_size * selections[i]), pcrvals[selections[i]]->digests[0].buffer, digest_size) != 0)
			return -EINVAL;
	}
	return 0;
}

int
main(void)
{
	static const struct {
		const char *id;
		TPM2_ALG_ID alg;
	} tab[] = {
	    {"sha1", TPM2_ALG_SHA1},
	    {"sha256", TPM2_ALG_SHA256},
	    {"sha384", TPM2_ALG_SHA384},
	    {"sha512", TPM2_ALG_SHA512},
	};

	TPML_DIGEST *pcrvals[4][32];
	int pcrlen[4];
	ESYS_CONTEXT *ctx;
	TSS2_RC r;
	size_t i;
	int j;

	r = Esys_Initialize(&ctx, NULL, NULL);
	check("Esys_Initialize", r);

	for (i = 0; i < nelem(tab); i++) {
		pcrlen[i] = get_pcr_values(ctx, tab[i].alg, pcrvals[i]);
		if (pcrlen[i] < 0)
			fatal("Failed to read PCR group %d\n", i);

		gen_c_array(tab[i].id, pcrvals[i], pcrlen[i]);
	}

	for (i = 0; i < nelem(pcrvals); i++) {
		for (j = 0; j < pcrlen[i]; j++)
			Esys_Free(pcrvals[i][j]);
	}
	Esys_Finalize(&ctx);

	return 0;
}
