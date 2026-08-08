// Example driver to use the custom methods defined

#include <vxWorks.h>
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>
#include <custom.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <hwif/methods/custom.h>

typedef Custom (*getcustomfn)(VXB_DEV_ID, Custom *);
typedef int (*intincfn)(VXB_DEV_ID, int);
typedef void (*showmefn)(VXB_DEV_ID);

LOCAL atomic_int loaded;

LOCAL int
probe(VXB_DEV_ID dev)
{
	if (!atomic_exchange(&loaded, 1))
		return OK;
	return ERROR;
}

LOCAL STATUS
attach(VXB_DEV_ID dev)
{
	return OK;
}

LOCAL void *
getptr(VXB_DEV_ID dev)
{
	printf("%s(dev=%p)\n", __func__, dev);
	return NULL;
}

LOCAL STATUS
getstatus(VXB_DEV_ID dev)
{
	printf("%s(dev=%p)\n", __func__, dev);
	return OK;
}

LOCAL Custom
getcustom(VXB_DEV_ID dev, Custom *custom)
{
	printf("%s(dev=%p, custom=%p {ix=%d iy=%d iz=%d})\n",
	       __func__, dev, custom, custom->ix, custom->iy, custom->iz);

	return (Custom){.ix = 199};
}

LOCAL int
intinc(VXB_DEV_ID dev, int value)
{
	printf("%s(dev=%p, value=%d)\n", __func__, dev, value);
	return value + 1;
}

LOCAL void
showme(VXB_DEV_ID dev)
{
	printf("%s(dev=%p)\n", __func__, dev);
}

LOCAL VXB_DRV_METHOD customdev[] = {
    {VXB_DEVMETHOD_CALL(vxbDevProbe), probe},
    {VXB_DEVMETHOD_CALL(vxbDevAttach), attach},

    {VXB_DEVMETHOD_CALL(getptr), (FUNCPTR)getptr},
    {VXB_DEVMETHOD_CALL(getstatus), (FUNCPTR)getstatus},
    {VXB_DEVMETHOD_CALL(getcustom), (FUNCPTR)getcustom},
    {VXB_DEVMETHOD_CALL(intinc), (FUNCPTR)intinc},
    {VXB_DEVMETHOD_CALL(showme), (FUNCPTR)showme},

    VXB_DEVMETHOD_END,
};

VXB_DRV customdrv = {
    {NULL},
    "custom",
    "Custom Method driver",
    VXB_BUSID_FDT,
    0,
    0,
    customdev,
    NULL,
};

STATUS
customdrvadd(void)
{
	return vxbDrvAdd(&customdrv);
}

void
customdrvtest(void)
{
	getcustomfn f_getcustom;
	intincfn f_intinc;
	showmefn f_showme;

	VXB_DEV_ID dev;
	Custom custom;

	dev = vxbDevAcquireByName("custom", 0);
	if (!dev)
		return;

	f_getcustom = (getcustomfn)vxbDevMethodFind(dev, VXB_DEVMETHOD_CALL(getcustom));
	f_intinc = (intincfn)vxbDevMethodFind(dev, VXB_DEVMETHOD_CALL(intinc));
	f_showme = (showmefn)vxbDevMethodFind(dev, VXB_DEVMETHOD_CALL(showme));

	memset(&custom, 0, sizeof(custom));
	custom.ix = 10;
	custom.iy = 23;
	custom.iz = 145;
	custom = f_getcustom(dev, &custom);
	printf("custom: %d %d %d\n", custom.ix, custom.iy, custom.iz);
	printf("intinc: %d\n", f_intinc(dev, 10));
	f_showme(dev);

	vxbDevRelease(dev);
}
