#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <tss2/tss2_esys.h>

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

const char *
error_code(int r)
{
	switch (r) {
	case TSS2_RC_SUCCESS:
		return "success";
	case TSS2_ESYS_RC_BAD_REFERENCE:
		return "bad reference";
	case TSS2_BASE_RC_BAD_CONTEXT:
		return "bad context";
	case TSS2_ESYS_RC_MEMORY:
		return "out of memory";
	case TSS2_ESYS_RC_BAD_SEQUENCE:
		return "async operation already pending";
	case TSS2_ESYS_RC_INSUFFICIENT_RESPONSE:
		return "tpm returned short data";
	case TSS2_ESYS_RC_MALFORMED_RESPONSE:
		return "tpm return malformed data";
	case TSS2_ESYS_RC_RSP_AUTH_FAILED:
		return "authentication failed";
	default:
		return "unknown error";
	}
}

int
test_random(ESYS_CONTEXT *ctx, size_t size)
{
	TPM2B_DIGEST *rnd;
	int i, r;

	printf("Testing random generator\n");

	// random data only returns a multiple of supported size, not arbitrary size
	// reflected in the size field of the digest structure return value
	r = Esys_GetRandom(ctx, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, size, &rnd);
	if (r != TSS2_RC_SUCCESS) {
		printf("Esys_GetRandom: Failed to get random data: %s\n", error_code(r));
		return -1;
	}

	printf("size %d\n", rnd->size);
	for (i = 0; i < rnd->size; i++) {
		if ((i & 0xf) == 0xf)
			printf("\n");
		printf("%02x ", rnd->buffer[i]);
	}
	printf("\n\n");
	free(rnd);

	return 0;
}

int
test_stir_random(ESYS_CONTEXT *ctx, size_t len)
{
	TPM2B_SENSITIVE_DATA sns;
	int r;

	printf("Testing Stir Random\n");
	printf("Max Buffer Size: %d\n", TPM2_MAX_SYM_DATA);
	printf("\n\n");

	memset(&sns, 0, sizeof(sns));
	sns.size = min(len, TPM2_MAX_SYM_DATA);
	r = Esys_StirRandom(ctx, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, &sns);
	if (r != TSS2_RC_SUCCESS) {
		printf("Esys_StirRandom: Failed to stir data: %s\n", error_code(r));
		return -1;
	}

	return 0;
}

int
test_self_test(ESYS_CONTEXT *ctx, TPMI_YES_NO full)
{
	int r;

	printf("Testing Self-Test\n");
	r = Esys_SelfTest(ctx, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, full);
	if (r != TSS2_RC_SUCCESS) {
		printf("Esys_SelfTest: %s\n", error_code(r));
		return -1;
	}
	return 0;
}

int
test_clock(ESYS_CONTEXT *ctx)
{
	TPMS_TIME_INFO *tm;
	int r;

	printf("Testing Clock\n");
	r = Esys_ReadClock(ctx, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, &tm);
	if (r != TSS2_RC_SUCCESS) {
		printf("Esys_ReadClock: %s\n", error_code(r));
		return -1;
	}

	printf("%lu\n", tm->time);

	free(tm);
	return 0;
}

int
main(void)
{
	ESYS_CONTEXT *ctx;
	TSS2_RC r;
	int i;

	r = Esys_Initialize(&ctx, NULL, NULL);
	if (r != TSS2_RC_SUCCESS)
		errx(1, "Esys_Initialize: Failed to initialize: %s", error_code(r));

	for (i = 0; i < 10; i++)
		test_random(ctx, 1 << i);

	test_stir_random(ctx, 32);
	test_stir_random(ctx, 128);

	test_self_test(ctx, 1);

	test_clock(ctx);

	Esys_Finalize(&ctx);
	return 0;
}
