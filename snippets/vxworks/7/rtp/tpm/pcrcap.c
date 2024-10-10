#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <err.h>
#include <tss2/tss2_esys.h>

int
main(void)
{
	ESYS_CONTEXT *ctx;
	TPMI_YES_NO moredata;
	TPMS_CAPABILITY_DATA *capdata;
	TPML_PCR_SELECTION *pcrl;
	TPMS_PCR_SELECTION *pcrs;
	UINT32 i, j;
	int r;

	r = Esys_Initialize(&ctx, NULL, NULL);
	if (r != TSS2_RC_SUCCESS)
		errx(1, "Failed to initialize TPM: %#x", r);

	r = Esys_GetCapability(ctx, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, TPM2_CAP_PCRS, 0, 1, &moredata, &capdata);
	if (r != TSS2_RC_SUCCESS)
		errx(1, "Failed to get PCR capability: %#x", r);

	pcrl = &capdata->data.assignedPCR;
	printf("PCR Capability: %#x\n", capdata->capability);
	printf("Number of selections: %d\n", pcrl->count);
	for (i = 0; i < pcrl->count; i++) {
		pcrs = &pcrl->pcrSelections[i];
		printf("PCR Selection %u\n", i);
		printf("Hash: %#x\n", pcrs->hash);
		printf("Size: %#x\n", pcrs->sizeofSelect);
		printf("Bitmap:\n");
		for (j = 0; j < pcrs->sizeofSelect; j++)
			printf("%x ", pcrs->pcrSelect[j]);
		printf("\n");
	}

	Esys_Free(capdata);
	Esys_Finalize(&ctx);

	return 0;
}
