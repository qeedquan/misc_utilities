// defines an nv index

#include "common.h"

typedef struct {
	int size;
} NV;

void nvopt(Option *o, int c);

NV nv = {
    .size = TPM2_MAX_NV_BUFFER_SIZE,
};

Option opt = {
    .hierarchy = ESYS_TR_RH_OWNER,
    .extraopts = "s:",
    .handler = nvopt,
    .userdata = &nv,
};

void
nvopt(Option *o, int c)
{
	NV *p;

	p = o->userdata;
	switch (c) {
	case 's':
		p->size = atoi(optarg);
		break;
	}
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
	unsigned long nvindex;
	ESYS_TR nvhandle;
	int i;

	parseopt(&opt, &argc, &argv);
	if (argc < 1)
		usage();

	ctx = newctx(&opt);

	TPM2B_AUTH auth = {0};
	TPM2B_NV_PUBLIC pub = {
	    .size = 0,
	    .nvPublic = {
	        .nvIndex = TPM2_NV_INDEX_FIRST,
	        .nameAlg = TPM2_ALG_SHA256,
	        .attributes = TPMA_NV_OWNERWRITE |
	                      TPMA_NV_AUTHWRITE |
	                      TPMA_NV_AUTHREAD |
	                      TPMA_NV_OWNERREAD,
	        .authPolicy = {
	            .size = 0,
	            .buffer = {0},
	        },
	        .dataSize = nv.size,
	    },
	};

	for (i = 0; i < argc; i++) {
		nvindex = strtoul(argv[i], NULL, 0);
		if (nvindex < TPM2_NV_INDEX_FIRST)
			nvindex += TPM2_NV_INDEX_FIRST;
		printf("Defining %#lx with size %d\n", nvindex, nv.size);

		pub.nvPublic.nvIndex = nvindex;
		r = Esys_NV_DefineSpace(ctx, opt.hierarchy, ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE, &auth, &pub, &nvhandle);
		if (r != TSS2_RC_SUCCESS)
			printf("Failed to define %#lx: %d\n", nvindex, r);
	}

	Esys_Finalize(&ctx);

	return 0;
}
