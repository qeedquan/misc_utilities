// delete an nv index

#include "common.h"

#define MAX_CAP_HANDLES (TPM2_NV_INDEX_LAST - TPM2_NV_INDEX_FIRST + 1)

Option opt = {
    .hierarchy = ESYS_TR_RH_OWNER,
};

void
usage(void)
{
	fprintf(stderr, "usage: [options] <nvindex> ...\n");
	exit(2);
}

void
wipeout(ESYS_CONTEXT *ctx)
{
	long nvindex;
	ESYS_TR nvhandle;
	TPMS_CAPABILITY_DATA *cap;
	TPMI_YES_NO more;
	TSS2_RC r;
	size_t i;

	do {
		r = Esys_GetCapability(ctx, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, TPM2_CAP_HANDLES,
		                       TPM2_NV_INDEX_FIRST, MAX_CAP_HANDLES, &more, &cap);
		check("Esys_GetCapability", r);

		for (i = 0; i < cap->data.handles.count; i++) {
			nvindex = cap->data.handles.handle[i];
			printf("Undefining %#lx:\n", nvindex);

			r = Esys_TR_FromTPMPublic(ctx, cap->data.handles.handle[i], ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, &nvhandle);
			check("Esys_TR_FromTPMPublic", r);

			r = Esys_NV_UndefineSpace(ctx, opt.hierarchy, nvhandle, ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE);
			if (r != TSS2_RC_SUCCESS)
				printf("Failed to undefine %#lx: %d\n", nvindex, r);
		}

		Esys_Free(cap);
	} while (more);
}

int
main(int argc, char *argv[])
{
	ESYS_CONTEXT *ctx;
	TSS2_RC r;
	ESYS_TR nvhandle;
	unsigned long nvindex;
	int i;

	parseopt(&opt, &argc, &argv);
	if (argc < 1)
		usage();

	ctx = newctx(&opt);

	if (!strcmp(argv[0], "all")) {
		wipeout(ctx);
		goto out;
	}

	for (i = 0; i < argc; i++) {
		nvindex = strtoul(argv[i], NULL, 0);
		if (nvindex < TPM2_NV_INDEX_FIRST)
			nvindex += TPM2_NV_INDEX_FIRST;

		printf("Undefining %#lx\n", nvindex);

		Esys_TR_FromTPMPublic(ctx, nvindex, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, &nvhandle);
		r = Esys_NV_UndefineSpace(ctx, opt.hierarchy, nvhandle, ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE);
		if (r != TSS2_RC_SUCCESS)
			printf("Failed to undefine %#lx: %d\n", nvindex, r);
	}

out:
	Esys_Finalize(&ctx);
	return 0;
}
