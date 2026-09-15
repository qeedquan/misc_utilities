#ifndef _XDEBUG_H_
#define _XDEBUG_H_

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/IoLib.h>
#include <Library/PrintLib.h>

static inline EFIAPI void
serialprint(CONST CHAR8 *Str)
{
	UINTN i;

	for (i = 0; Str[i]; i++)
		IoWrite8(0x3f8, Str[i]);
}

static inline EFIAPI UINTN
xprint(CONST CHAR8 *Format, ...)
{
	CHAR8 Buffer[512];
	VA_LIST Marker;
	UINTN Length;

	VA_START(Marker, Format);
	Length = AsciiVSPrint(Buffer, sizeof(Buffer) / sizeof(Buffer[0]), Format, Marker);
	VA_END(Marker);
	serialprint(Buffer);
	return Length;
}

#endif
