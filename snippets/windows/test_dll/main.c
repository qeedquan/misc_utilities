#include <windows.h>

// __declspec(dllimport) is optional for function declarations, but it can generate more efficient
// code if this keyword is used. However, this is mandatory for importing executable to
// access DLL public data symbol and object
__declspec(dllimport) void SayHello(const char *name);
__declspec(dllimport) void Question(const char *str, int val);
void Midterm(void);

void Fido(void);

// need __declspec(dllimport) to get global from dll
__declspec(dllimport) int Global;

int
main(void)
{
	Fido();
	SayHello("kiwi");
	Question("undo", 1);
	Midterm();
	printf("Global = %d\n", Global);
	return 0;
}