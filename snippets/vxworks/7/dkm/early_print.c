// early printing for x86
#include <vxWorks.h>
#include <sysLib.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

static void
serialprint(const char *str)
{
	int port;
	size_t i;

	port = 0x3f8;
	for (i = 0; str[i]; i++)
		sysOutByte(port, str[i]);
}

static int
stoib(char *buf, unsigned long long val, unsigned long long base)
{
	static const char alphabet[] = "0123456789abcdef";
	int i, j, n;
	char c;

	n = 0;
	do {
		buf[n++] = alphabet[val % base];
		val /= base;
	} while (val);

	for (i = 0; i < n / 2; i++) {
		j = n - i - 1;
		c = buf[i];
		buf[i] = buf[j];
		buf[j] = c;
	}
	buf[n] = '\0';

	return n;
}

void
printval(const char *name, unsigned long long val, unsigned long long base)
{
	char buf[32];

	stoib(buf, val, base);
	serialprint(name);
	serialprint(": ");
	serialprint(buf);
	serialprint("\n");
}

void
printptr(const char *name, void *ptr)
{
	printval(name, (uintptr_t)ptr, 16);
}

void
printstr(const char *str)
{
	serialprint(str);
}

void
printline(const char *str)
{
	serialprint(str);
	serialprint("\n");
}

void
sysmemdesc(void)
{
	static const struct {
		char name[32];
		MEM_DESC_TYPE type;
	} desc[] = {
	    {"MEM_DESC_RAM", MEM_DESC_RAM},
	    {"MEM_DESC_USER_RESERVED_RAM", MEM_DESC_USER_RESERVED_RAM},
	    {"MEM_DESC_PM_RAM", MEM_DESC_PM_RAM},
	    {"MEM_DESC_DMA32_RAM", MEM_DESC_DMA32_RAM},
	    {"MEM_DESC_ROM", MEM_DESC_ROM},
	};

	struct phys_mem_desc p;
	size_t i;
	int n;

	printstr("\n");
	for (i = 0; i < nelem(desc); i++) {
		for (n = 0; sysMemDescGet(desc[i].type, n, &p) != ERROR; n++) {
			if (n == 0)
				printline(desc[i].name);
			printval("\tvirtual_address    :", p.virtualAddr, 16);
			printval("\tphysical address   :", p.physicalAddr, 16);
			printval("\tlength             :", p.len, 16);
			printval("\tinitial state mask :", p.initialStateMask, 16);
			printval("\tinitial state      :", p.initialState, 16);
			printstr("\n");
		}
		printstr("\n");
	}
	printstr("\n");
}
