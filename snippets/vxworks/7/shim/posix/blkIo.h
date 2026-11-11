#ifndef _BLKIO_H_
#define _BLKIO_H_

typedef struct {
	FUNCPTR bd_blkRd;
	FUNCPTR bd_blkWrt;
	FUNCPTR bd_ioctl;
	FUNCPTR bd_reset;
	FUNCPTR bd_statusChk;
	BOOL bd_removable;
	ULONG bd_nBlocks;
	ULONG bd_bytesPerBlk;
	ULONG bd_blksPerTrack;
	ULONG bd_nHeads;
	int bd_retry;
	int bd_mode;
	BOOL bd_readyChanged;
} BLK_DEV;

#endif
