#include <stdio.h>
#include <string.h>
#include <err.h>
#include <tss2/tss2_esys.h>

int
main(int argc, char *argv[])
{
	ESYS_CONTEXT *ctx;
	TPM2B_MAX_BUFFER *out;
	TPMI_YES_NO full;
	TPM2_RC rc;
	int r;

	full = 0;
	if (argc >= 2 && !strcmp(argv[1], "-f"))
		full = 1;

	r = Esys_Initialize(&ctx, NULL, NULL);
	if (r != TSS2_RC_SUCCESS)
		errx(1, "Failed to initialize TPM: %#x", r);

	r = Esys_SelfTest(ctx, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, full);
	if (r != TSS2_RC_SUCCESS)
		errx(1, "Failed to self test: %#x", r);

	r = Esys_GetTestResult(ctx, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, &out, &rc);
	if (r != TSS2_RC_SUCCESS)
		errx(1, "Failed to get test result: %#x", r);

	printf("Test result: %#x\n", rc);
	printf("Test result size: %d\n", out->size);

	Esys_Finalize(&ctx);

	return 0;
}
