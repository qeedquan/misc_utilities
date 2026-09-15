#include <vxWorks.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <ipdnsc.h>

void
print_hostent(struct hostent *ent)
{
	char buf[128];
	size_t i;

	printf("Hostname: %s\n", ent->h_name);
	printf("Address type: %d\n", ent->h_addrtype);
	printf("Address length: %d\n", ent->h_length);
	for (i = 0; ent->h_aliases[i]; i++)
		printf("Aliases: %s\n", ent->h_aliases[i]);
	for (i = 0; ent->h_addr_list[i]; i++) {
		if (inet_ntop(ent->h_addrtype, ent->h_addr_list[i], buf, sizeof(buf)))
			printf("Address: %s\n", buf);
		else
			printf("         %s\n", buf);
	}
	printf("\n");
}

const char *
dnsc_error_string(int code)
{
	switch (code) {
	case IPDNSC_NO_ADDRESS:
		return "IPDNSC_NO_ADDRESS";
	case IPDNSC_HOST_NOT_FOUND:
		return "IPDNSC_HOST_NOT_FOUND";
	case IPDNSC_NO_RECOVERY:
		return "IPDNSC_NO_RECOVERY";
	case IPDNSC_TRY_AGAIN:
		return "IPDNSC_TRY_AGAIN";
	}
	return "UNKNOWN";
}

STATUS
dnsc_name_test(const char *name)
{
	struct hostent *ent;
	int err;
	STATUS status;

	status = OK;
	err = 0;
	ent = ipdnsc_getipnodebyname(name, AF_INET, 0, &err);
	if (!ent) {
		printf("Error querying by name: %s\n", dnsc_error_string(err));
		goto error;
	}

	print_hostent(ent);

	if (0) {
	error:
		status = ERROR;
	}

	ipdnsc_freehostent(ent);

	return status;
}
