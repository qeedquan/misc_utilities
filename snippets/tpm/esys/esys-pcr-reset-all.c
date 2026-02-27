// try to reset pcrs that is resettable
// most pcr are not resettable, only 16 and maybe 23 are allowable

#include "common.h"

int
main(void)
{
	ESYS_CONTEXT *ctx;
	TSS2_RC r;
	int i;

	r = Esys_Initialize(&ctx, NULL, NULL);
	check("Esys_Initialize", r);

	for (i = 0; i < TPM2_MAX_PCRS; i++) {
		r = Esys_PCR_Reset(ctx, i, ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE);
		if (r == TSS2_RC_SUCCESS)
			printf("Cleared PCR %d\n", i);
	}

	Esys_Finalize(&ctx);

	return 0;
}
