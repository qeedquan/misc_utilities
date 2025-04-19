// read firmware data back

#include "common.h"

int
main(void)
{
	TPM2B_MAX_BUFFER *data;
	ESYS_CONTEXT *ctx;
	TSS2_RC r;
	uint32_t seq;

	r = Esys_Initialize(&ctx, NULL, NULL);
	check("Esys_Initialize", r);

	seq = 0;
	r = Esys_FirmwareRead(ctx, ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE, seq, &data);
	check("Esys_FirmwareRead", r);

	hexdump(data->buffer, data->size);

	Esys_Free(data);
	Esys_Finalize(&ctx);

	return 0;
}
