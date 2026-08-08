#ifndef _IOUTIL_H_
#define _IOUTIL_H_

#ifndef IOUNIT
#define IOUNIT 65536
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>

void fatal(const char *, ...);

void *xmalloc(size_t);
void *xcalloc(size_t, size_t);
void *xrealloc(void *, size_t);

void *readfile(const char *, size_t *);
int writefile(const char *, const void *, size_t);

void *freadall(FILE *, size_t *);

#endif
