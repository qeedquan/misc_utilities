#ifndef _VXB_RESOURCE_LIB_H_
#define _VXB_RESOURCE_LIB_H_

enum {
	VXB_RES_MEMORY = 1,
	VXB_RES_IO = 2,
	VXB_RES_IRQ = 3,
};

enum {
	VXB_RES_FLAG_MAPPED = 0x01000000,
	VXB_RES_FLAG_NONPOSTED = 0x02000000,
};

typedef struct vxbResource {
	SL_NODE node;
	UINT32 id;
	void *pRes;
	VXB_DEV_ID owner;
	VXB_KEY vxbSerial;
} VXB_RESOURCE;

#endif
