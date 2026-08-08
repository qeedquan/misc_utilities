#include <stdio.h>

// by default the loader has these symbols as markers of where one section ends
// we can define more using the linker

// end of text section
extern char etext[];

// end of data section
extern char edata[];

// end of all sections
extern char end[];

int
main(void)
{
	printf("etext = %p\n", etext);
	printf("edata = %p\n", edata);
	printf("end   = %p\n", end);
	return 0;
}
