#include <vxWorks.h>
#include <stdio.h>
#include <sysLib.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

void
print_mem_desc(int type, const char *desc)
{
	struct phys_mem_desc p;
	unsigned long long len;
	int i;

	len = 0;
	printf("Region: %s\n", desc);
	printf("-----------------------------\n");
	printf("%-20s %-20s %-12s %-12s %-12s\n", "Virtual", "Physical", "Length", "State Mask", "State");
	for (i = 0;; i++) {
		if (sysMemDescGet(type, i, &p) == ERROR)
			break;

		printf("%#-20llx %#-20llx %#-12zx %#-12x %#-12x\n",
		       (unsigned long long)p.virtualAddr, (unsigned long long)p.physicalAddr, p.len, p.initialStateMask, p.initialState);

		len += p.len;
	}
	printf("\n");
	printf("Total Length: %llu bytes (%llu mb)\n", len, len / (1024ULL * 1024ULL));
	printf("\n\n");
}

void
dump_mem_descs(void)
{
	static const struct {
		int type;
		const char *desc;
	} tab[] = {
	    {MEM_DESC_RAM, "RAM"},
	    {MEM_DESC_USER_RESERVED_RAM, "User Reserved RAM"},
	    {MEM_DESC_PM_RAM, "PM RAM"},
	    {MEM_DESC_DMA32_RAM, "DMA32 RAM"},
	    {MEM_DESC_ROM, "ROM"},

#ifdef _WRS_CONFIG_MPU_RTP
	    {MEM_DESC_KV_RAM, "KV RAM"},
	    {MEM_DESC_RTP_RAM, "RTP RAM"},
#endif

#ifdef _WRS_CONFIG_MMULESS_KERNEL
	    {MEM_DESC_SHARED_RAM, "Shared RAM"},
#endif

	};

	size_t i;

	for (i = 0; i < nelem(tab); i++)
		print_mem_desc(tab[i].type, tab[i].desc);
}
