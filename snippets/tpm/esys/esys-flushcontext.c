// flush context for reloading slots

#include "common.h"

#define MAX_CAP_HANDLES 1024

typedef struct {
	int properties[3];
} Flush;

Flush flush;

void flushopt(Option *, int);

Option opt = {
    .extraopts = "tls",
    .handler = flushopt,
    .userdata = &flush,
};

void
flushopt(Option *o, int c)
{
	Flush *f;

	f = o->userdata;
	switch (c) {
	case 't':
		f->properties[0] = TPM2_TRANSIENT_FIRST;
		break;
	case 'l':
		f->properties[1] = TPM2_LOADED_SESSION_FIRST;
		break;
	case 's':
		f->properties[2] = TPM2_ACTIVE_SESSION_FIRST;
		break;
	}
}

int
main(int argc, char *argv[])
{
	TPMS_CAPABILITY_DATA *capdata;
	TPMI_YES_NO more;
	ESYS_CONTEXT *ctx;
	ESYS_TR obj;
	TSS2_RC r;
	size_t i, j;

	parseopt(&opt, &argc, &argv);
	ctx = newctx(&opt);

	for (i = 0; i < nelem(flush.properties); i++) {
		if (!flush.properties[i])
			continue;

		do {
			r = Esys_GetCapability(ctx, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, TPM2_CAP_HANDLES,
			                       flush.properties[i], MAX_CAP_HANDLES, &more, &capdata);
			check("Esys_GetCapability", r);

			for (j = 0; j < capdata->data.handles.count; j++) {
				printf("Flushing handle %lx\n", (long)capdata->data.handles.handle[j]);

				r = Esys_TR_FromTPMPublic(ctx, capdata->data.handles.handle[j], ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, &obj);
				check("Esys_TR_FromTPMPublic", r);

				r = Esys_FlushContext(ctx, obj);
				check("Esys_FlushContext", r);
			}

			Esys_Free(capdata);
		} while (more);
	}

	Esys_Finalize(&ctx);

	return 0;
}
