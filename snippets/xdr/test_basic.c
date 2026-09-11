#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <rpc/rpc.h>

void
test_mem()
{
	char mem[1024];
	XDR ex, dx;
	int ei, di;
	char es[32], ds[32];
	char *sp;

	xdrmem_create(&ex, mem, sizeof(mem), XDR_ENCODE);
	xdrmem_create(&dx, mem, sizeof(mem), XDR_DECODE);

	for (ei = 0; ei <= 50; ei++) {
		assert(xdr_int(&ex, &ei));
	}

	strcpy(es, "hello");
	sp = &es[0];
	assert(xdr_string(&ex, &sp, strlen(es)));

	for (ei = 0; ei <= 50; ei++) {
		assert(xdr_int(&dx, &di));
		assert(ei == di);
	}
	sp = &ds[0];
	assert(xdr_string(&dx, &sp, sizeof(ds)));
	assert(!strcmp(es, ds));
}

int
main()
{
	test_mem();

	return 0;
}
