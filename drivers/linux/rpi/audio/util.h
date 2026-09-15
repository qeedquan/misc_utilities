#ifndef _UTIL_H_
#define _UTIL_H_

#include <stddef.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

size_t pgroundup(size_t size);
void millisleep(long ms);
void *readfile(const char *name, size_t *size);

#endif
