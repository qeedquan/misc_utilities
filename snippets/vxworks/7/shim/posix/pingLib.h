#ifndef _PINGLIB_H_
#define _PINGLIB_H_

#include "types/vxTypes.h"
#include "hostLib.h"
#include "inetLib.h"

enum {
	PING_OPT_SILENT = 0x1,
	PING_OPT_DONTROUTE = 0x2,
	PING_OPT_DEBUG = 0x4,
	PING_OPT_NOHOST = 0x8,
};

enum {
	PING_MAXPACKET = 65536,
};

enum {
	PING_INTERVAL = 1,
	PING_TMO = 1,
};

typedef struct PING_STAT PING_STAT;

struct PING_STAT {
	PING_STAT *statNext;
	int pingFd;
	char toHostName[MAXHOSTNAMELEN + 2];
	char toInetName[INET_ADDR_LEN];
	u_char *bufTx;
	u_char *bufRx;
	struct icmp *pBufIcmp;
	ulong_t *pBufTime;
	int dataLen;
	int numPacket;
	int numTx;
	int numRx;
	int idRx;
	int clkTick;
	int tMin;
	int tMax;
	int tSum;
	int flags;
	int timing;
};

#define S_pingLib_NOT_INITIALIZED (M_pingLib | 1)
#define S_pingLib_TIMEOUT (M_pingLib | 2)

STATUS pingLibInit(void);

STATUS ping(char *host, int numPackets, ulong_t options);

#endif
