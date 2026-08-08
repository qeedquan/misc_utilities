#ifndef _VX_CHECKSUM_H_
#define _VX_CHECKSUM_H_

uint8_t vxCrc7(uint8_t crc, const uint8_t *data, size_t len);
uint8_t vxCrc8(const char *data, size_t len);

uint16_t vxCrcCcittFalse(uint16_t crc, const uint8_t *data, size_t len);
uint16_t vxCrcCcitt(uint16_t crc, const uint8_t *data, size_t len);
uint16_t vxCrc16(uint16_t crc, const uint8_t *data, size_t len);

uint32_t vxCrc32(uint32_t crc, const uint8_t *data, size_t len);

#endif
