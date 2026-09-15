#ifndef _ENDIAN_H_
#define _ENDIAN_H_

#include <stdint.h>

uint16_t be16dec(const void *);
uint32_t be32dec(const void *);
uint64_t be64dec(const void *);

uint16_t le16dec(const void *);
uint32_t le32dec(const void *);
uint64_t le64dec(const void *);

void be16enc(void *, uint16_t);
void be32enc(void *, uint32_t);
void be64enc(void *, uint64_t);

void le16enc(void *, uint16_t);
void le32enc(void *, uint32_t);
void le64enc(void *, uint64_t);

uint16_t bswap16(uint16_t);
uint32_t bswap32(uint32_t);
uint64_t bswap64(uint64_t);

#endif
