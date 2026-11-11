// link with ws2_32.lib
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <initguid.h>
#include <WinSock2.h>
#include <ws2bth.h>

DEFINE_GUID(g_guidServiceClass, 0xb62c4e8d, 0x62cc, 0x404b, 0xbb, 0xbf, 0xbf, 0x3e, 0x3b, 0xbb, 0x13, 0x74);

struct {
	size_t bufsz;
	long delay;
	char prot[64];
	int port;
} opt = {
	.bufsz = 512,
	.delay = 0,
	.prot = "rfcomm:0",
};

int
str2ba(const char *addr, BTH_ADDR *bdaddr)
{
	ULONG b[6];
	int i, rv;

	rv = sscanf_s(addr, "%02x:%02x:%02x:%02x:%02x:%02x",
	              &b[0], &b[1], &b[2], &b[3], &b[4], &b[5]);
	if (rv != 6)
		return -1;

	*bdaddr = 0;
	for (i = 0; i < 6; i++)
		*bdaddr = (*bdaddr << 8) | (b[i] & 0xff);
	return 0;
}

void
fatal(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fprintf(stderr, "\n");
	exit(1);
}

void
usage(void)
{
	fprintf(stderr, "usage: [options] server\n");
	fprintf(stderr, "                 client addr\n");
	fprintf(stderr, "  -b <size> buffer size (default: %zu)\n", opt.bufsz);
	fprintf(stderr, "  -d <ms>   delay (default: %ld)\n", opt.delay);
	fprintf(stderr, "  -h        show this message\n");
	fprintf(stderr, "  -p <prot> use protocol (default: %s)\n", opt.prot);
	fprintf(stderr, "\nAvailable protocols:\n\"rfcomm:port\"\n");
	exit(2);
}

void
parseargs(int *argc, char ***argv)
{
	int i, j, k;

	for (i = 1; i < *argc; i += k) {
		k = 1;
		if ((*argv)[i][0] != '-')
			break;
		for (j = 1; (*argv)[i][j]; j++) {
			switch ((*argv)[i][j]) {
			case 'b':
				opt.bufsz = atol((*argv)[i + k++]);
				break;
			case 'd':
				opt.delay = atol((*argv)[i + k++]);
				break;
			case 'p':
				snprintf(opt.prot, sizeof(opt.prot), "%s", (*argv)[i + k++]);
				break;
			case 'h':
			default:
				usage();
				break;
			}
		}
	}
	*argc -= i;
	*argv += i;

	if (*argc < 1)
		usage();
}

void
server(char *buf)
{
	SOCKADDR_BTH rc_addr = { 0 };
	void *saddr;
	int saddrlen;
	int sfd, cfd;
	long nr;
	uintmax_t tnr, tot;

	sfd = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
	if (sfd == INVALID_SOCKET)
		fatal("socket: %d", WSAGetLastError());

	if (opt.port <= 0)
		opt.port = 5;

	rc_addr.addressFamily = AF_BTH;
	rc_addr.port = opt.port;
	saddr = &rc_addr;
	saddrlen = sizeof(rc_addr);

	if (bind(sfd, saddr, saddrlen) == SOCKET_ERROR)
		fatal("bind: %d", WSAGetLastError());

	if (getsockname(sfd, saddr, &saddrlen) == SOCKET_ERROR)
		fatal("getsockname: %d", WSAGetLastError());

	if (listen(sfd, SOMAXCONN) == SOCKET_ERROR)
		fatal("listen: %d", WSAGetLastError());

	printf("listening on addr %#lx\n", (long)rc_addr.btAddr);

	cfd = accept(sfd, NULL, NULL);
	if (cfd == INVALID_SOCKET)
		fatal("accept: %d", WSAGetLastError());

	printf("accepted a connection\n");
	tnr = tot = 0;
	for (;;) {
		nr = recv(cfd, buf, opt.bufsz, 0);
		if (nr == SOCKET_ERROR) {
			printf("recv: %d\n", WSAGetLastError());
			break;
		}

		tnr += nr;
		tot += opt.bufsz;
		printf("%ld %ju %ju %f\n", nr, tnr, tot, tnr * 1.0 / tot);
		Sleep(opt.delay);
	}

	closesocket(sfd);
}

void
client(const char *addr, char *buf)
{
	SOCKADDR_BTH rc_addr = { 0 };
	void *saddr;
	int saddrlen;
	int fd;
	long nw;
	uintmax_t tnw, tot;

	if (opt.port <= 0)
		opt.port = 5;

	rc_addr.addressFamily = AF_BTH;
	rc_addr.serviceClassId = g_guidServiceClass;
	rc_addr.port = opt.port;
	if (str2ba(addr, &rc_addr.btAddr) < 0)
		fatal("str2ba: failed to convert bluetooth address");
	// service class id is if we want to bind to a service, it is a replacement for port
	// if we wanted to use it, we need to set the port to 0

	printf("connecting to %#llx on port %d\n", rc_addr.btAddr, rc_addr.port);

	saddr = &rc_addr;
	saddrlen = sizeof(rc_addr);
	fd = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
	if (fd == INVALID_SOCKET)
		fatal("socket: %d", WSAGetLastError());

	if (connect(fd, (struct sockaddr *)saddr, saddrlen) == SOCKET_ERROR)
		fatal("connect: %d", WSAGetLastError());

	tnw = tot = 0;
	for (;;) {
		nw = send(fd, buf, opt.bufsz, 0);
		if (nw < 0) {
			printf("send: %d\n", WSAGetLastError());
			break;
		}
		tnw += nw;
		tot += opt.bufsz;
		printf("%ld %ju %ju %f\n", nw, tnw, tot, tnw * 1.0 / tot);
		Sleep(opt.delay);
	}

	closesocket(fd);
}

int
main(int argc, char *argv[])
{
	WSADATA WSAData = { 0 };
	char *buf;

	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
	parseargs(&argc, &argv);
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
		fatal("failed to init winsock");

	if (sscanf_s(opt.prot, "rfcomm:%i", &opt.port) != 1)
		fatal("unsupported protocol %s", opt.prot);

	assert(buf = calloc(opt.bufsz, sizeof(*buf)));
	if (!strcmp(argv[0], "server"))
		server(buf);
	else if (!strcmp(argv[0], "client")) {
		if (argc < 2)
			usage();
		client(argv[1], buf);
	} else
		usage();
	free(buf);

	return 0;
}
