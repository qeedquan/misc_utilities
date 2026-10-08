#include <vxWorks.h>
#include <stdio.h>
#include <private/secSecretP.h>

STATUS
secvault_dump(void)
{
	char name[SEC_SECRET_KEY_ID_MAX + 1];
	void *handle;

	handle = secSecretOpen();
	if (!handle) {
		printf("Failed to open secret handle\n");
		return ERROR;
	}

	for (;;) {
		if (secSecretRead(handle, name, sizeof(name)) != OK)
			break;
		printf("%s\n", name);
	}
	secSecretClose(handle);

	return OK;
}
