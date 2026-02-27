#ifndef _VXB_ACCESS_H_
#define _VXB_ACCESS_H_

typedef struct vxbRegHandle {
	int vxbResFd;
	VXB_RESOURCE *pVxbRes;
} VXB_REG_HANDLE;

STATUS vxbRegMap(VXB_RESOURCE *res);
STATUS vxbRegUnmap(VXB_RESOURCE *res);

STATUS vxbRegUnmapAll(VXB_DEV_ID);

UINT8 vxbRead8(void *, UINT8 *);
UINT16 vxbRead16(void *, UINT16 *);
UINT32 vxbRead32(void *, UINT32 *);
void vxbWrite8(void *, UINT8 *, UINT8);
void vxbWrite16(void *, UINT16 *, UINT16);
void vxbWrite32(void *, UINT32 *, UINT32);

#endif
