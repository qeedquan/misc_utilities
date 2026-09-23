/*

Example of loading DLL dynamically.
Need to provide definitions that match the DLL definitions.

*/

#include <iostream>
#include <cstdio>
#include <windows.h>
#include "../basic.h"

typedef void (__cdecl *printfn)(const char *);

void fatal(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
	printf("\n");
	exit(1);
}

int main()
{
	// This will call C++ constructors for global objects in the DLL
	auto lib = LoadLibrary("dll/mydll.dll");
	if (!lib)
		fatal("Failed to load DLL");

	auto hello = (printfn) GetProcAddress(lib, "SayHello");
	
	// C++ global/function names will be mangled
	auto basic = (Basic *) GetProcAddress(lib, "?GlobalBasic@@3UBasic@@A");
	
	if (!hello || !basic)
		fatal("Failed to load functions/globals");

	hello("World!");
	basic->ival[0] = 133;
	basic->fval[0] = 34.5;
	basic->cval[0] = 'z';
	basic->dump();

	FreeLibrary(lib);

	return 0;
}
