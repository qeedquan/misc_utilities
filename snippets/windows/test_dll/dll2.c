#include <stdio.h>

void Fido(void);

// needed to export global variables from dll
__declspec(dllexport) int Global;

__declspec(dllexport)
void
Midterm(void)
{
	printf("%s()\n", __func__);
	Fido();
	Global = 321;
}