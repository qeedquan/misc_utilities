#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <err.h>
#include <netdb.h>
#include <arpa/inet.h>

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

int
main(int argc, char *argv[])
{
	struct hostent *ent;

	if (argc < 2)
		errx(1, "usage: host");

	ent = gethostbyname(argv[1]);
	if (!ent)
		errx(1, "Failed to query: %s\n", strerror(errno));

	print_hostent(ent);

	return 0;
}
