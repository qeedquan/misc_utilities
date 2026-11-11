#include <vxWorks.h>
#include <stdio.h>
#include <endian.h>

int
main(int argc, char *argv[])
{
	uint8_t b[] = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8};

	be16dec(b);
	be32dec(b);
	be64dec(b);

	le16dec(b);
	le32dec(b);
	le64dec(b);

	return 0;
}
