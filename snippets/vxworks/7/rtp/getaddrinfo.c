#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <errnoLib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sockLib.h>
#include <unistd.h>

void
dump_addrinfo(struct addrinfo *ai)
{
	struct sockaddr *sa;
	char addr[64];

	sa = ai->ai_addr;

	switch (sa->sa_family) {
	case AF_INET:
		inet_ntop(AF_INET, &(((struct sockaddr_in *)sa)->sin_addr), addr, sizeof(addr));
		break;
	case AF_INET6:
		inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)sa)->sin6_addr), addr, sizeof(addr));
		break;
	default:
		strcpy(addr, "unknown");
		break;
	}

	printf("\n");
	printf("flags: %d\n", ai->ai_flags);
	printf("family: %d\n", ai->ai_family);
	printf("socktype: %d\n", ai->ai_socktype);
	printf("protocol: %d\n", ai->ai_protocol);
	printf("address: %s\n", addr);
	printf("\n");
}

void
knock(const char *name, const char *service)
{
	struct addrinfo hints, *results, *res;
	int fd;
	int rv;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_protocol = IPPROTO_TCP;

	printf("knocking %s %s\n", name, service);
	rv = getaddrinfo(name, service, &hints, &results);
	if (rv != 0) {
		printf("getaddrinfo: %d %s errno=%#x\n", rv, gai_strerror(rv), errno);
		return;
	}

	for (res = results; res != NULL; res = res->ai_next) {
		dump_addrinfo(res);

		fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (fd < 0) {
			printf("socket: errno=%#x\n", errno);
			continue;
		}

		rv = connect(fd, res->ai_addr, res->ai_addrlen);
		if (rv != 0)
			printf("connect: errno=%#x %s\n", errno, strerror(errno));

		close(fd);
	}

	freeaddrinfo(results);
}

int
main(int argc, char *argv[])
{
	const char *name, *service;

	name = "8.8.8.8";
	service = "443";
	if (argc >= 2)
		name = argv[1];
	if (argc >= 3)
		service = argv[2];

	knock(name, service);
	return 0;
}
