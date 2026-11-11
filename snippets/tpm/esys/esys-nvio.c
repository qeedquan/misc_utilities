// delete an nv index

#include "common.h"

typedef struct {
	int off;
	int size;
	int readonly;
} NV;

NV nv = {
    .size = TPM2_MAX_NV_BUFFER_SIZE / 2,
};

void nvopt(Option *, int);

Option opt = {
    .hierarchy = ESYS_TR_RH_OWNER,
    .extraopts = "o:s:r:",
    .handler = nvopt,
    .userdata = &nv,
};

void
nvopt(Option *o, int c)
{
	switch (c) {
	case 'o':
		nv.off = atoi(optarg);
		break;
	case 's':
		nv.size = atoi(optarg);
		break;
	case 'r':
		nv.readonly = 1;
		break;
	}

	nv.off = clamp(nv.off, 0, TPM2_MAX_NV_BUFFER_SIZE);
	nv.size = clamp(nv.size, 0, TPM2_MAX_NV_BUFFER_SIZE);

	(void)o;
}

void
usage(void)
{
	fprintf(stderr, "usage: [options] <nvindex> ...\n");
	exit(2);
}

int
main(int argc, char *argv[])
{
	ESYS_CONTEXT *ctx;
	TSS2_RC r;
	ESYS_TR nvhandle;
	TPM2B_MAX_NV_BUFFER nvin, *nvout;
	unsigned long nvindex;
	int i;

	parseopt(&opt, &argc, &argv);
	if (argc < 1)
		usage();

	srand(time(NULL));
	ctx = newctx(&opt);

	for (i = 0; i < argc; i++) {
		nvindex = strtoul(argv[i], NULL, 0);
		if (nvindex < TPM2_NV_INDEX_FIRST)
			nvindex += TPM2_NV_INDEX_FIRST;

		printf("Testing IO %#lx with size %d offset %d\n", nvindex, nv.size, nv.off);

		Esys_TR_FromTPMPublic(ctx, nvindex, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, &nvhandle);

		nvin.size = nv.size;
		genrand(nvin.buffer, sizeof(nvin.buffer));

		if (!nv.readonly) {
			r = Esys_NV_Write(ctx, opt.hierarchy, nvhandle, ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE, &nvin, nv.off);
			if (r != TSS2_RC_SUCCESS)
				printf("Failed to write %#lx: %d\n", nvindex, r);
		}

		nvout = NULL;
		r = Esys_NV_Read(ctx, opt.hierarchy, nvhandle, ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE, nv.size, nv.off, &nvout);
		if (r != TSS2_RC_SUCCESS) {
			printf("Failed to read %#lx: %d\n", nvindex, r);
			continue;
		}

		if (nv.readonly) {
			printf("Read %d\n", nvout->size);
			hexdump(nvout->buffer, nvout->size);
		} else if (nvin.size != nvout->size || memcmp(nvin.buffer, nvout->buffer, nvin.size) != 0)
			printf("Mismatch IO: wrote %d read %d\n", nvin.size, nvout->size);

		Esys_Free(nvout);
	}

	Esys_Finalize(&ctx);

	return 0;
}
