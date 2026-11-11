// Different APIs for allocating memory

#include <stdio.h>
#include <windows.h>

// Allocate memory from the heap
void
testheap(void)
{
	HANDLE handle;
	unsigned char *data;
	
	handle = GetProcessHeap();
	data = HeapAlloc(handle, HEAP_ZERO_MEMORY, 1024);
	memset(data, 0xff, 1024);
	printf("%d %d\n", data[0], data[1]);
	HeapFree(handle, 0, data);
}

int
main(void)
{
	testheap();
	return 0;
}
