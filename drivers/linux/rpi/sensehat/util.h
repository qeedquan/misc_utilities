#ifndef _UTIL_H_
#define _UTIL_H_

#include <math.h>
#include <stddef.h>
#include <stdint.h>

#define DEG2RAD (M_PI / 180.0f)

#define nelem(x) (sizeof(x) / sizeof(x[0]))

int64_t seload(uint64_t measured, size_t size);
uint64_t leload(uint8_t *buf, size_t size);
int finddevs(char ***devs, size_t *len, const char *path, const char *sig, const char *devpath, int ndp);
int queryi2cdev(const char *dev, const int *addrs, int reg, int id);
void millisleep(long ms);
float vmag(float v[3]);

#endif
