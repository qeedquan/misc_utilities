#include <depcom.h>
#include "depa.h"

void
depa(void)
{
	printf("%s:%d\n", __func__, __LINE__);
	depcom();
}
