#include <vxWorks.h>
#include <vmLib.h>

STATUS
vmStateGet(VM_CONTEXT_ID context, VIRT_ADDR pageAddr, UINT *pState)
{
	return OK;
}

size_t
vmPageSizeGet(void)
{
	return 4096;
}
