// clears values from the hierachy

#include "common.h"

Option opt = {
    .hierarchy = ESYS_TR_RH_LOCKOUT,
};

int
main(int argc, char *argv[])
{
	ESYS_CONTEXT *ctx;
	TSS2_RC r;

	parseopt(&opt, &argc, &argv);

	r = Esys_Initialize(&ctx, NULL, NULL);
	check("Esys_Initialize", r);

	r = Esys_Clear(ctx, opt.hierarchy, ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE);
	check("Esys_Clear", r);

	Esys_Finalize(&ctx);

	return 0;
}
