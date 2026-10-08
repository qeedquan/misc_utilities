// get mac address of the socket
// we open to connect
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <err.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <linux/if.h>

int
main(void)
{
	struct sockaddr addr;
	struct sockaddr_in *in, *pn, saddr;
	struct ifreq ifr, *it, *ie;
	struct ifconf ifc;
	struct hostent *ent;
	socklen_t addrlen;
	int sfd;
	char buf[1024];
	unsigned char mac[6];

	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd < 0)
		errx(1, "socket: %s", strerror(errno));

	ent = gethostbyname("google.com");
	if (ent == NULL)
		errx(1, "gethostbyname: %s", strerror(errno));

	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	memcpy(&saddr.sin_addr.s_addr, ent->h_addr_list[0], ent->h_length);
	saddr.sin_port = htons(80);

	// connect the socket
	if (connect(sfd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
		errx(1, "connect: %s", strerror(errno));

	// if getsockname is called before connection
	// the address won't be bound and get 0 back
	addrlen = sizeof(addr);
	if (getsockname(sfd, &addr, &addrlen) < 0)
		errx(1, "getsockname: %s", strerror(errno));

	// get all net interface
	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = buf;
	if (ioctl(sfd, SIOCGIFCONF, &ifc) < 0)
		errx(1, "ioctl: %s", strerror(errno));
	it = ifc.ifc_req;
	ie = it + (ifc.ifc_len / sizeof(struct ifreq));

	// loop through all interface
	// and only if the ip address matches the socket
	// ip we print out the mac for that interface
	assert(addr.sa_family == AF_INET);
	in = (struct sockaddr_in *)&addr;
	for (; it != ie; it++) {
		if (it->ifr_addr.sa_family != AF_INET)
			continue;

		pn = (struct sockaddr_in *)&it->ifr_addr;
		if (pn->sin_addr.s_addr == in->sin_addr.s_addr) {
			strcpy(ifr.ifr_name, it->ifr_name);
			if (ioctl(sfd, SIOCGIFHWADDR, &ifr) != 0)
				continue;

			memcpy(mac, ifr.ifr_hwaddr.sa_data, sizeof(mac));
			printf("%s\n", it->ifr_name);
			printf("%s\n", inet_ntoa(pn->sin_addr));
			printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
			       mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		}
	}

	close(sfd);
	return 0;
}
