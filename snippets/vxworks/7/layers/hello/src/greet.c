#include <stdio.h>
#include "hello.h"

int
greet(const char *s)
{
	hello(s);
	return goodbye();
}
