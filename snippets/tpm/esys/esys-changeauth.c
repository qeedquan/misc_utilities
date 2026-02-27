// use changeauth to set passwords on the hierarchy
// if there is a password already set, need to provide a password for authentication before we can change the password

#include "common.h"

Option opt = {
    .hierarchy = ESYS_TR_RH_OWNER,
};

void
usage(void)
{
	fprintf(stderr, "usage: [options] <pass>\n");
	exit(2);
}

int
main(int argc, char *argv[])
{
	ESYS_CONTEXT *ctx;
	TPM2B_AUTH auth;
	size_t len;
	TSS2_RC r;

	printf("Max password size: %zu\n", sizeof(auth.buffer));

	parseopt(&opt, &argc, &argv);
	if (argc < 1)
		usage();

	len = strlen(argv[0]);
	if (sizeof(auth.buffer) < len)
		fatal("Password too long");

	auth.size = len;
	memcpy(auth.buffer, argv[0], len);

	ctx = newctx(&opt);

	r = Esys_HierarchyChangeAuth(ctx, opt.hierarchy, ESYS_TR_PASSWORD, ESYS_TR_NONE, ESYS_TR_NONE, &auth);
	check("Esys_HierarchyChangeAuth", r);

	Esys_Finalize(&ctx);

	return 0;
}
