#include <iostream>
#include <cstdio>
#include <windows.h>
#include "basic.h"

extern "C" {

// __declspec(dllimport) is optional for function declarations, but it can generate more efficient code if this keyword is used.
// However, this is mandatory for importing executable to access DLL public data symbol and object
// When compiling in C++, need to wrap this around an extern C block

__declspec(dllimport) void SayHello(const char *name);
__declspec(dllimport) void Question(const char *str, int val);
void Midterm(void);

void Fido(void);

// need __declspec(dllimport) to get global from dll
__declspec(dllimport) int Global;

}

__declspec(dllimport) void test_basic();
__declspec(dllimport) Basic GlobalBasic;

int main()
{
	Fido();
	SayHello("kiwi");
	Question("undo", 1);
	Midterm();
	printf("Global = %d\n", Global);
	std::cout << "C++" << std::endl;
	test_basic();
	GlobalBasic.dump();
	return 0;
}