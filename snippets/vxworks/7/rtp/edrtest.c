#include <stdio.h>
#include <edrLib.h>

int
main(int argc, char *argv[])
{
	printf("EDR Flag: %#x\n", edrFlagsGet());
	printf("Debug mode: %d\n", edrIsDebugMode());

	EDR_USER_INFO_INJECT(FALSE, "USER_INFO");
	EDR_USER_WARNING_INJECT(FALSE, "USER_WARNING");
	EDR_USER_FATAL_INJECT(FALSE, "USER_FATAL");
	EDR_RTP_FATAL_INJECT(FALSE, "RTP_FATAL");

	return 0;
}
