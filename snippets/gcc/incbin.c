#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <err.h>

int
main(void)
{
	__asm__(
	    ".pushsection .rodata.test_section\n\t"
	    ".globl test_start;\n\t"
	    ".globl test_end;\n\t"
	    ".balign 4\n\t"
	    "test_start: .incbin \"test.bin\"\n\t"
	    "test_end: .4byte 0;\n\t"
	    ".balign 4\n\t"
	    ".popsection\n\t");
	extern const uint8_t test_start;
	extern const uint8_t test_end;

	const uint8_t *start = &test_start;
	const uint8_t *end = &test_end;

	assert(end >= start);
	printf("%p %p\n", (void *)start, (void *)end);

	FILE *fp = fopen("out.bin", "wb");
	if (!fp)
		err(1, "fopen");
	fwrite(start, end - start, 1, fp);
	fclose(fp);

	return 0;
}
