#ifndef _VMLIB_H_
#define _VMLIB_H_

typedef void *VM_CONTEXT_ID;

STATUS vmStateGet(VM_CONTEXT_ID context, VIRT_ADDR pageAddr, UINT *pState);
size_t vmPageSizeGet(void);

#endif
