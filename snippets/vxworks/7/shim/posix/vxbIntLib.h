#ifndef _VXINTLIB_H_
#define _VXINTLIB_H_

typedef struct {
	VXB_RESOURCE *pRes;
	UINT32 node;
	UINT32 *pProp;
	UINT32 numProp;
} VXB_INTR_ENTRY;

typedef struct {
	UINT32 hVec;
	UINT32 lVec;
	VOIDFUNCPTR pIsr;
	void *pArg;
	UINT32 flag;
	VXB_INTR_ENTRY *pVxbIntrEntry;
} VXB_RESOURCE_IRQ;

#endif
