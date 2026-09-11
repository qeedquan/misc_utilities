#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include <endian.h>
#include <vxFdtLib.h>

char *
fdtget(int *totalsize)
{
	char *fdt;

	fdt = vxFdtDtbAddressGet();
	if (!fdt) {
		printf("Failed to get FDT\n");
		return NULL;
	}
	*totalsize = le32dec(fdt + 4);
	return fdt;
}

void
fdtinfo(void)
{
	const char *cfg;
	char bootargs[128];
	char *fdt;
	int totalsize;
	int rngoff, rnglen;

	fdt = fdtget(&totalsize);
	if (!fdt)
		return;

	if (vxFdtBootargsGet(bootargs, sizeof(bootargs)) != OK)
		bootargs[0] = '\0';

	cfg = vxFdtStdoutCfgGet();
	if (!cfg)
		cfg = "<nil>";

	rnglen = 0;
	rngoff = vxFdtPathOffset("/chosen");
	if (rngoff > 0) {
		vxFdtPropGet(rngoff, "rng-seed", &rnglen);
	}

	printf("FDT Info\n");
	printf("\tAddress:       %p\n", fdt);
	printf("\tTotal size:    %d\n", totalsize);
	printf("\tSystem Model:  %s\n", vxFdtSysModel());
	printf("\tStandard DTB:  %d\n", vxFdtStdDtb());
	printf("\tStdout ID:     %d\n", vxFdtStdoutGet());
	printf("\tStdout Config: %s\n", cfg);
	printf("\tBootline:      %s\n", bootargs);
	if (rnglen)
		printf("\tRNG Length:    %d\n", rnglen);
	printf("\n");
}

STATUS
fdtdump(const char *name)
{
	STATUS status;
	FILE *fp;
	char *fdt;
	int totalsize;

	status = OK;
	fp = NULL;
	fdt = fdtget(&totalsize);
	if (!fdt)
		goto error;

	fp = fopen(name, "wb+");
	if (!fp) {
		printf("Failed to open file: %s\n", strerror(errno));
		goto error;
	}

	if (fwrite(fdt, totalsize, 1, fp) != 1) {
		printf("Failed to write: %s\n", strerror(errno));
		goto error;
	}

	if (0) {
	error:
		status = ERROR;
	}

	if (fp)
		fclose(fp);

	return status;
}
