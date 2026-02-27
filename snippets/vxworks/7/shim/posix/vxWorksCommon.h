#ifndef _VXWORKS_COMMON_H_
#define _VXWORKS_COMMON_H_

#define FOREVER for (;;)

#define NONE -1
#define EOS '\0'

#define NO_WAIT 0
#define WAIT_FOREVER 0xFFFFFFFFU

#define STD_IN 0
#define STD_OUT 1
#define STD_ERR 2

#define VX_READ 0
#define VX_WRITE 1

#define WORDSWAP(x) (MSW(x) | (LSW(x) << 16))

#define LLSB(x) ((x) & 0xff)
#define LNLSB(x) (((x) >> 8) & 0xff)
#define LNMSB(x) (((x) >> 16) & 0xff)
#define LMSB(x) (((x) >> 24) & 0xff)
#define LONGSWAP(x) ((LLSB(x) << 24) |  \
	             (LNLSB(x) << 16) | \
	             (LNMSB(x) << 8) |  \
	             (LMSB(x)))

#define OFFSET(structure, member) ((long)&(((structure *)0)->member))

#define MEMBER_SIZE(structure, member) (sizeof(((structure *)0)->member))

#endif
