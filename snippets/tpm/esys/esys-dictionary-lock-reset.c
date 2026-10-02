// reset lockout

#include "common.h"

int
main(void)
{
	ESYS_CONTEXT *ctx;
	TSS2_RC r;

	r = Esys_Initialize(&ctx, NULL, NULL);
	check("Esys_Initialize", r);

	r = Esys_DictionaryAttackLockReset(ctx, ESYS_TR_RH_LOCKOUT, ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE);
	check("Esys_DictionaryAttackLockReset", r);

	Esys_Finalize(&ctx);

	return 0;
}
