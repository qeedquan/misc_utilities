#ifndef _VXBUS_H_
#define _VXBUS_H_

struct vxbDev {
	struct vxbDev *parent;
	char name[128];
	void *ivar;
};

struct vxbResource {
	int _;
};

typedef char *VXB_BUSTYPE_ID;
typedef struct vxbDev *VXB_DEV_ID;
typedef struct vxbResource VXB_RESOURCE;
typedef VXB_DEV_ID VXB_DEV_HANDLE;

#define VXB_BUSID_MATCH(type, bus) TRUE

#define VXB_BUSID_ROOT "root"
#define VXB_BUSID_NONE "none"

VXB_BUSTYPE_ID vxbDevClassGet(VXB_DEV_ID dev);
char *vxbDevNameGet(VXB_DEV_ID dev);
VXB_DEV_ID vxbDevParent(VXB_DEV_ID dev);

void *vxbDevIvarsGet(VXB_DEV_ID dev);
void vxbDevIvarsSet(VXB_DEV_ID dev, void *ivar);

void vxbAcquireByMatch(BOOL (*match)(VXB_DEV_ID, void *), void *ctx);

#endif
