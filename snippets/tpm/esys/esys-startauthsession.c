#include "common.h"

typedef struct {
	TPM2B_NONCE nonce;
	TPM2_SE session_type;
	TPMT_SYM_DEF symmetric;
	TPMI_ALG_HASH hash;
} AI;

AI ai = {
    .session_type = TPM2_SE_HMAC,
    .symmetric = {
        .mode = {
            .aes = TPM2_ALG_CFB,
        },
        .keyBits = {
            .aes = 128,
        },
        .algorithm = TPM2_ALG_AES,
    },
    .hash = TPM2_ALG_SHA256,
};

Option opt = {
    .hierarchy = ESYS_TR_RH_OWNER,
    .userdata = &ai,
};

void
usage(void)
{
	fprintf(stderr, "usage: [options]\n");
	exit(2);
}

// dummy session doesn't encrypt
// run this in the simulator to see the plain text traffic
ESYS_TR
dummy_session(ESYS_CONTEXT *ctx)
{
	ESYS_TR session;
	TSS2_RC r;

	r = Esys_StartAuthSession(ctx, ESYS_TR_NONE, ESYS_TR_NONE,
	                          ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE,
	                          &ai.nonce,
	                          ai.session_type,
	                          &ai.symmetric, ai.hash, &session);
	check("Esys_StartAuthSession", r);
	return session;
}

ESYS_TR
encrypted_session(ESYS_CONTEXT *ctx)
{
	TPMA_SESSION sessionAttributes;
	ESYS_TR session;
	TSS2_RC r;

	r = Esys_StartAuthSession(ctx, ESYS_TR_NONE, ESYS_TR_NONE,
	                          ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE,
	                          &ai.nonce,
	                          ai.session_type,
	                          &ai.symmetric, ai.hash, &session);
	check("Esys_StartAuthSession", r);

	sessionAttributes = (TPMA_SESSION_DECRYPT |
	                     TPMA_SESSION_ENCRYPT |
	                     TPMA_SESSION_CONTINUESESSION);
	r = Esys_TRSess_SetAttributes(ctx, session, sessionAttributes, 0xff);
	check("Esys_TRSess_SetAttributes", r);

	return session;
}

int
main(int argc, char *argv[])
{
	TPM2B_AUTH password = {
	    .size = 5,
	    .buffer = "hello",
	};

	ESYS_TR session;
	ESYS_CONTEXT *ctx;
	int r;

	parseopt(&opt, &argc, &argv);
	ctx = newctx(&opt);

	srand(time(NULL));
	ai.nonce.size = 20;
	for (int i = 0; i < ai.nonce.size; i++)
		ai.nonce.buffer[i] = rand() & 0xff;

	session = encrypted_session(ctx);

	printf("Session ID: %x\n", session);

	TPM2B_MAX_NV_BUFFER nvin, *nvout;
	ESYS_TR nvhandle;
	TPM2B_AUTH auth = {0};
	TPM2B_NV_PUBLIC pub = {
	    .size = 0,
	    .nvPublic = {
	        .nvIndex = TPM2_NV_INDEX_FIRST,
	        .nameAlg = TPM2_ALG_SHA256,
	        .attributes = TPMA_NV_OWNERWRITE |
	                      TPMA_NV_AUTHWRITE |
	                      TPMA_NV_AUTHREAD |
	                      TPMA_NV_OWNERREAD,
	        .authPolicy = {
	            .size = 0,
	            .buffer = {0},
	        },
	        .dataSize = 1024,
	    },
	};

	nvin.size = pub.nvPublic.dataSize / 4;
	for (size_t i = 0; i < nvin.size; i++)
		nvin.buffer[i] = i & 0xff;

	r = Esys_NV_DefineSpace(ctx, opt.hierarchy, session, ESYS_TR_NONE, ESYS_TR_NONE, &auth, &pub, &nvhandle);
	if (r != TSS2_RC_SUCCESS) {
		printf("Failed to define NV index %#x: %d\n", pub.nvPublic.nvIndex, r);
		Esys_TR_FromTPMPublic(ctx, pub.nvPublic.nvIndex, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, &nvhandle);
	}

	r = Esys_NV_Write(ctx, opt.hierarchy, nvhandle, session, ESYS_TR_NONE, ESYS_TR_NONE, &nvin, 0);
	if (r != TSS2_RC_SUCCESS)
		printf("Failed to write %#x: %d\n", pub.nvPublic.nvIndex, r);

	r = Esys_NV_Read(ctx, opt.hierarchy, nvhandle, session, ESYS_TR_NONE, ESYS_TR_NONE, nvin.size, 0, &nvout);
	if (r != TSS2_RC_SUCCESS)
		printf("Failed to read %#x: %d\n", pub.nvPublic.nvIndex, r);

	if (!nvout || (nvin.size != nvout->size || memcmp(nvin.buffer, nvout->buffer, nvout->size)))
		printf("Mismatch NV IO\n");

	r = Esys_HierarchyChangeAuth(ctx, opt.hierarchy, session, ESYS_TR_NONE, ESYS_TR_NONE, &password);
	check("Esys_HierarchyChangeAuth", r);

	Esys_Free(nvout);
	Esys_FlushContext(ctx, session);
	Esys_Finalize(&ctx);

	return 0;
}
