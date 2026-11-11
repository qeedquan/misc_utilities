// use tpm to hash some file data, the result should match using sha256sum on the file

#include "common.h"

Option opt = {
    .hierarchy = ESYS_TR_RH_OWNER,
    .hashalg = TPM2_ALG_SHA256,
};

int
main(int argc, char *argv[])
{
	TPM2B_AUTH auth = {
	    .size = 20,
	    .buffer = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20},
	};

	TPM2B_MAX_BUFFER buffer;

	TPM2B_DIGEST *result = NULL;
	TPMT_TK_HASHCHECK *validation = NULL;

	ESYS_CONTEXT *ctx;
	ESYS_TR seqhandle;
	TSS2_RC r;

	uint8_t *in;
	size_t inlen;
	size_t i;

	if (argc < 2) {
		printf("usage: file\n");
		exit(2);
	}

	in = read_file(argv[1], &inlen);
	assert(in);

	r = Esys_Initialize(&ctx, NULL, NULL);
	check("Esys_Initialize", r);

	r = Esys_HashSequenceStart(ctx, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, &auth, opt.hashalg, &seqhandle);
	check("Esys_HashSequence", r);

	r = Esys_TR_SetAuth(ctx, seqhandle, &auth);
	check("Esys_Tr_SetAuth", r);

	for (i = 0; i < inlen;) {
		buffer.size = min(sizeof(buffer.buffer), inlen - i);
		memcpy(buffer.buffer, in + i, buffer.size);

		r = Esys_SequenceUpdate(ctx, seqhandle, ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE, &buffer);
		check("Esys_SequenceUpdate", r);
		i += buffer.size;
	}

	buffer.size = 0;
	r = Esys_SequenceComplete(ctx, seqhandle, ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE, &buffer, opt.hierarchy, &result, &validation);
	check("Esys_SequenceComplete", r);

	hexdump(result->buffer, result->size);
	printf("tag %#x hierarchy %#x\n", validation->tag, validation->hierarchy);

	hexdump(auth.buffer, auth.size);
	hexdump(validation->digest.buffer, validation->digest.size);

	free(in);
	Esys_Free(result);
	Esys_Free(validation);
	Esys_Finalize(&ctx);

	return 0;
}
