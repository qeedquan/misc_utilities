// https://stackoverflow.com/questions/19656946/why-setunhandledexceptionfilter-cannot-capture-some-exception-but-addvectoredexc
#include <tchar.h>
#include <windows.h>
#include <stdio.h>

LONG WINAPI
VectoredExceptionHandler(PEXCEPTION_POINTERS pExceptionInfo)
{
	printf("%s(pExceptionInfo = %p) -> (Code = %x)\n",
	       __func__, (void *)pExceptionInfo, (UINT)pExceptionInfo->ExceptionRecord->ExceptionCode);
	return EXCEPTION_CONTINUE_SEARCH;
}

LONG WINAPI
TerminateExceptionHandler(PEXCEPTION_POINTERS pExceptionInfo)
{
	printf("%s(pExceptionInfo = %p) -> (Code = %x)\n",
	       __func__, (void *)pExceptionInfo, (UINT)pExceptionInfo->ExceptionRecord->ExceptionCode);

	// segfault exception
	if (pExceptionInfo->ExceptionRecord->ExceptionCode == 0xc0000005) {
		printf("Caught segfault, exiting!\n");
		ExitProcess(1);

		// this will give it to the OS handler, which results in a segfault
		return EXCEPTION_EXECUTE_HANDLER;
	}
	return EXCEPTION_CONTINUE_EXECUTION;
}

DWORD WINAPI
SegfaultThread(LPVOID lpParam)
{
	int SleepTime = 5;

	printf("Segfaulting in %d seconds\n", SleepTime);
	Sleep(SleepTime * 1000);
	*((int *)NULL) = 0;
	return 0;
}

void
PrintExceptionCodes(void)
{
	printf("DBG_PRINTEXCEPTION_C                %lx\n", (unsigned long)DBG_PRINTEXCEPTION_C);
	printf("EXCEPTION_ACCESS_VIOLATION          %lx\n", (unsigned long)EXCEPTION_ACCESS_VIOLATION);
	printf("EXCEPTION_ARRAY_BOUNDS_EXCEEDED     %lx\n", (unsigned long)EXCEPTION_ARRAY_BOUNDS_EXCEEDED);
	printf("EXCEPTION_BREAKPOINT                %lx\n", (unsigned long)EXCEPTION_BREAKPOINT);
	printf("EXCEPTION_DATATYPE_MISALIGNMENT     %lx\n", (unsigned long)EXCEPTION_DATATYPE_MISALIGNMENT);
	printf("EXCEPTION_FLT_DENORMAL_OPERAND      %lx\n", (unsigned long)EXCEPTION_FLT_DENORMAL_OPERAND);
	printf("EXCEPTION_FLT_DIVIDE_BY_ZERO        %lx\n", (unsigned long)EXCEPTION_FLT_DIVIDE_BY_ZERO);
	printf("EXCEPTION_FLT_INEXACT_RESULT        %lx\n", (unsigned long)EXCEPTION_FLT_INEXACT_RESULT);
	printf("EXCEPTION_FLT_INVALID_OPERATION     %lx\n", (unsigned long)EXCEPTION_FLT_INVALID_OPERATION);
	printf("EXCEPTION_FLT_OVERFLOW              %lx\n", (unsigned long)EXCEPTION_FLT_OVERFLOW);
	printf("EXCEPTION_FLT_STACK_CHECK           %lx\n", (unsigned long)EXCEPTION_FLT_STACK_CHECK);
	printf("EXCEPTION_FLT_UNDERFLOW             %lx\n", (unsigned long)EXCEPTION_FLT_UNDERFLOW);
	printf("EXCEPTION_ILLEGAL_INSTRUCTION       %lx\n", (unsigned long)EXCEPTION_ILLEGAL_INSTRUCTION);
	printf("EXCEPTION_IN_PAGE_ERROR             %lx\n", (unsigned long)EXCEPTION_IN_PAGE_ERROR);
	printf("EXCEPTION_INVALID_DISPOSITION       %lx\n", (unsigned long)EXCEPTION_INVALID_DISPOSITION);
	printf("EXCEPTION_NONCONTINUABLE_EXCEPTION  %lx\n", (unsigned long)EXCEPTION_NONCONTINUABLE_EXCEPTION);
	printf("EXCEPTION_PRIV_INSTRUCTION          %lx\n", (unsigned long)EXCEPTION_PRIV_INSTRUCTION);
	printf("EXCEPTION_SINGLE_STEP               %lx\n", (unsigned long)EXCEPTION_SINGLE_STEP);
	printf("EXCEPTION_STACK_OVERFLOW            %lx\n", (unsigned long)EXCEPTION_STACK_OVERFLOW);
	printf("STATUS_UNWIND_CONSOLIDATE           %lx\n", (unsigned long)ERROR_UNWIND_CONSOLIDATE);
}

int
main(int argc, char *argv[])
{
	HANDLE Thread[1];

	PrintExceptionCodes();

	// make it the first handler to be called
	AddVectoredExceptionHandler(1, VectoredExceptionHandler);

	// make it last to be called
	AddVectoredExceptionHandler(0, TerminateExceptionHandler);

	RaiseException(0xc0000374, 0, 0, NULL);
	RaiseException(0x00065432, 0, 0, NULL);

	// segfault exception
	Thread[0] = CreateThread(NULL, 0, SegfaultThread, NULL, 0, NULL);
	WaitForMultipleObjects(1, Thread, TRUE, INFINITE);

	return 0;
}
