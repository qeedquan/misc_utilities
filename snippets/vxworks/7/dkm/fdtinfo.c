#include <vxWorks.h>
#include <stdio.h>
#include <inttypes.h>
#include <endian.h>
#include <vxFdtLib.h>

void
fdtinfo(void)
{
	const char *cfg;
	char bootargs[128];
	char *ptr;
	void *fdt;
	int totalsize;
	int rngoff, rnglen;

	fdt = vxFdtDtbAddressGet();

	ptr = fdt;
	totalsize = le32dec(ptr + 4);

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
