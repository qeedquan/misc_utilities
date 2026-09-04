#include <stdio.h>
#include <windows.h>

int
main(void)
{
	const char *subkey = "Software\\Testing";
	const char *regvalue = "overload";
	DWORD value;
	HKEY key;

	value = GetProcessId(GetCurrentProcess());
	RegCreateKeyA(HKEY_CURRENT_USER, subkey, &key);
	RegSetKeyValueA(HKEY_CURRENT_USER, subkey, regvalue, REG_DWORD, &value, sizeof(value));
	RegCloseKey(key);
	printf("%#x\n", value);

	return 0;
}
