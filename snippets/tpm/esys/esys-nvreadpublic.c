// dump nv indices

#include "common.h"

#define MAX_CAP_HANDLES (TPM2_NV_INDEX_LAST - TPM2_NV_INDEX_FIRST + 1)

Option opt = {0};

int
main(int argc, char *argv[])
{
	TPM2B_NV_PUBLIC *nvpublic;
	TPM2B_NAME *nvname;
	ESYS_TR nvhandle;
	TPMS_CAPABILITY_DATA *cap;
	ESYS_CONTEXT *ctx;
	TPMI_YES_NO more;
	size_t i, j;
	TSS2_RC r;

	parseopt(&opt, &argc, &argv);

	r = Esys_Initialize(&ctx, NULL, NULL);
	check("Esys_Initialize", r);

	do {
		r = Esys_GetCapability(ctx, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, TPM2_CAP_HANDLES,
		                       TPM2_NV_INDEX_FIRST, MAX_CAP_HANDLES, &more, &cap);
		check("Esys_GetCapability", r);

		for (i = 0; i < cap->data.handles.count; i++) {
			printf("%#lx:\n", (long)cap->data.handles.handle[i]);

			r = Esys_TR_FromTPMPublic(ctx, cap->data.handles.handle[i], ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, &nvhandle);
			check("Esys_TR_FromTPMPublic", r);

			r = Esys_NV_ReadPublic(ctx, nvhandle, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, &nvpublic, &nvname);
			check("Esys_NV_ReadPublic", r);

			printf("  name: ");
			for (j = 0; j < nvname->size; j++)
				printf("%02x", nvname->name[j]);
			printf("\n");
			printf("  hash algorithm:\n");
			printf("    value: %#X\n", nvpublic->nvPublic.nameAlg);
			printf("  attributes:\n");
			printf("    value: %#X\n", nvpublic->nvPublic.attributes);
			printf("  size: %d\n", nvpublic->nvPublic.dataSize);
			printf("  policy (%#X bytes):\n", nvpublic->nvPublic.authPolicy.size);
			hexdump(nvpublic->nvPublic.authPolicy.buffer, nvpublic->nvPublic.authPolicy.size);
			printf("\n");

			Esys_Free(nvpublic);
			Esys_Free(nvname);
		}

		Esys_Free(cap);
	} while (more);

	Esys_Finalize(&ctx);

	return 0;
}
