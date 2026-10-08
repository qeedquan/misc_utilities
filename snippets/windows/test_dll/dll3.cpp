#include "basic.h"

__declspec(dllexport) Basic GlobalBasic;

__declspec(dllexport)
void test_basic()
{
	Basic b;
	b.dump();
}