#include <depcom.h>
#include "depb.h"

void
depb(void)
{
	printf("%s:%d\n", __func__, __LINE__);
	depcom();
}
