#include <vxWorks.h>
#include <hwif/vxBus.h>
#include "custom.h"

/*

All types need to be defined before method definitions, all arguments have to be named.
This file will be compiled during VSB build time and a header will be generated into hwif/methods.
Drivers which implement these custom methods can include the header file generated to get the definitions.

Method definitions need to follow the layout:
METHOD [return type] [function name](VXB_DEV_ID pDev, [type] arg0, [type] arg1 ...); default[return value] # "description"

*/

METHOD void * getptr(VXB_DEV_ID pDev); default[NULL] # "get pointer"
METHOD STATUS getstatus(VXB_DEV_ID pDev); default[OK] # "get status"
METHOD Custom getcustom(VXB_DEV_ID pDev, Custom * pCustom); default[{.fx=2, .fy=3}] # "get custom structure"
METHOD int intinc(VXB_DEV_ID pDev, int value); default[73] # "increment an integer"
METHOD void showme(VXB_DEV_ID pDev); default[void] # "show stuff"
