/* Include inside the VIP project */

#include <vxWorks.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cacheLib.h>
#include <sysDma.h>

STATUS
compare_buffer(char *buf0, char *buf1, int size)
{
	STATUS status;
	int i;

	status = OK;
	for (i = 0; i < size; i++) {
		if (buf0[i] != buf1[i]) {
			printf("Buffer mismatched: %x: %x %x\n", i, buf0[i], buf1[i]);
			status = ERROR;
		}
	}

	printf("Compare buffer: %s\n", (status == OK) ? "OK" : "FAIL");
	return status;
}

int
test_dma_basic(int chan, int size)
{
	DMA_DESCRIPTOR desc;
	DMA_ATTRIBUTES attr;
	STATUS status;
	char *dst, *src;
	int i;

	status = OK;
	dst = cacheDmaMalloc(size);
	src = cacheDmaMalloc(size);
	if (!dst || !src) {
		printf("Failed to allocate memory for DMA\n");
		goto error;
	}

	memset(dst, 0, size);
	for (i = 0; i < size; i++)
		src[i] = i & 0xff;

	memset(&desc, 0, sizeof(desc));
	desc.byteCount = size;
	desc.sourceAddr = (UINT32)src;
	desc.destAddr = (UINT32)dst;

	memset(&attr, 0, sizeof(attr));
	attr.userHandler = (void *)WAIT_FOREVER;

	if (sysDmaStart(chan, &desc, &attr, NULL) != OK) {
		printf("Failed to start DMA\n");
		goto error;
	}

	status = compare_buffer(src, dst, size);

	if (0) {
	error:
		status = ERROR;
	}

	if (dst)
		cacheDmaFree(dst);
	if (src)
		cacheDmaFree(src);

	return status;
}
