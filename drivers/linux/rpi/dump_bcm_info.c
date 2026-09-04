// cc -o dump_bcm_info dump_bcm_info.c -I/opt/vc/include -L/opt/vc/lib -lbcm_host
#define _GNU_SOURCE
#include <stdio.h>
#include <bcm_host.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

enum {
	BCM2835,
	BCM2836,
	BCM2837,
	BCM2838
};

const char *
get_processor_id_str(int id)
{
	switch (id) {
	case BCM2835:
		return "BCM2835";
	case BCM2836:
		return "BCM2836";
	case BCM2837:
		return "BCM2837";
	case BCM2838:
		return "BCM2838";
	default:
		return "UNKNOWN";
	}
}

// https://www.raspberrypi.org/documentation/hardware/raspberrypi/revision-codes/README.md
unsigned int
get_revision_code(void)
{
	unsigned int revision;
	FILE *fp;
	char buf[8192];

	revision = 0;
	fp = fopen("/proc/cpuinfo", "rt");
	if (fp == NULL)
		return revision;

	while (fgets(buf, sizeof(buf), fp)) {
		if (sscanf(buf, "Revision : %x", &revision) == 1)
			break;
	}

	fclose(fp);
	return revision;
}

int
get_processor_id(void)
{
	unsigned int revision;

	revision = get_revision_code();
	if (revision & 0x800000)
		return (revision & 0xf000) >> 12;
	return BCM2835;
}

int
get_model_type(void)
{
	static const unsigned char map[] = {
	    1,       // B rev 1.0  2
	    1,       // B rev 1.0  3
	    1,       // B rev 2.0  4
	    1,       // B rev 2.0  5
	    1,       // B rev 2.0  6
	    0,       // A rev 2    7
	    0,       // A rev 2    8
	    0,       // A rev 2    9
	    0, 0, 0, // unused  a,b,c
	    1,       // B  rev 2.0  d
	    1,       // B rev 2.0  e
	    1,       // B rev 2.0  f
	    3,       // B+ rev 1.2 10
	    6,       // CM1        11
	    2,       // A+ rev1.1  12
	    3,       // B+ rev 1.2 13
	    6,       // CM1        14
	    2        // A+         15
	};

	unsigned int revision;

	revision = get_revision_code();
	if (!revision)
		return 0;

	// new style revision, bit 23 guarantees it
	if (revision & 0x800000)
		return (revision & 0xff0) >> 4;

	revision &= 0xffffff;
	if (revision < 2 || revision > 21)
		return 0;

	return map[revision - 2];
}

unsigned long long
get_mem_size(void)
{
	static const unsigned long long mem[] = {
	    1ULL << 28,
	    1ULL << 29,
	    1ULL << 30,
	    1ULL << 31,
	    1ULL << 32,
	};
	unsigned int revision;

	revision = get_revision_code();
	if (revision & 0x800000) {
		revision = (revision >> 22) & 0xf;
		if (revision < nelem(mem))
			return mem[revision];
	}
	return mem[0];
}

void
dump_graphics(void)
{
	uint32_t screen, width, height;

	for (screen = 0; screen < 1; screen++) {
		graphics_get_display_size(screen, &width, &height);
		printf("Display #%d %dx%d\n", screen, width, height);
	}
}

void
dump_address(void)
{
	printf("Peripheral Address %#x\n", bcm_host_get_peripheral_address());
	printf("Peripheral Size    %#x\n", bcm_host_get_peripheral_size());
	printf("SDRAM Address      %#x\n", bcm_host_get_sdram_address());
}

void
dump_model(void)
{
	printf("Revision Code      %#x\n", get_revision_code());
	printf("Model Type         %#x\n", get_model_type());
	printf("Processor Type     %s\n", get_processor_id_str(get_processor_id()));
	printf("Memory Size        %llu Bytes\n", get_mem_size());
}

int
main(void)
{
	bcm_host_init();
	vc_gencmd_init();
	dump_graphics();
	dump_address();
	dump_model();
	bcm_host_deinit();
	vc_gencmd_stop();
	return 0;
}
