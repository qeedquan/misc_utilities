#include <stdio.h>
#include <windows.h>

int
main(void)
{
	SYSTEMTIME lt = {0};
	SYSTEMTIME st = {0};
	FILETIME ft = {0};
	LARGE_INTEGER li = {0};
	long long hns;

	GetLocalTime(&lt);
	GetSystemTime(&st);
	GetSystemTimeAsFileTime(&ft);
	
	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;
	hns = li.QuadPart;

	printf("Local Time         %02d:%02d:%02d\n", lt.wHour, lt.wMinute, lt.wSecond);
	printf("System Time (UTC)  %02d:%02d:%02d\n", st.wHour, st.wMinute, st.wSecond);
	printf("%lli hundreds of nanoseconds have elapsed since Windows API epoch\n", hns);
	
	return 0;
}