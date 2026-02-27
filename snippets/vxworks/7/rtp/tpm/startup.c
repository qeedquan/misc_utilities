#include <stdio.h>
#include <string.h>
#include <err.h>
#include <tss2/tss2_esys.h>

int
main(int argc, char *argv[])
{
	ESYS_CONTEXT *ctx;
	int state;
	int r;

	state = TPM2_SU_STATE;
	if (argc >= 2 && !strcmp(argv[1], "-c"))
		state = TPM2_SU_CLEAR;

	r = Esys_Initialize(&ctx, NULL, NULL);
	if (r != TSS2_RC_SUCCESS)
		errx(1, "Failed to initialize TPM: %#x", r);

	printf("Setting TPM state %#x\n", state);

	r = Esys_Startup(ctx, state);
	if (r != TSS2_RC_SUCCESS)
		errx(1, "Failed to set TPM startup state: %#x", r);

	Esys_Finalize(&ctx);

	return 0;
}
