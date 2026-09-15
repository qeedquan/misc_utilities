// test simple transfers
// need to be in pscan mode (accepting connections)
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <getopt.h>
#include <err.h>
#include <unistd.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/l2cap.h>

struct {
	size_t bufsz;
	time_t delay;
	char prot[64];
	int port;
	int retry;
} opt = {
	.bufsz = 512,
	.delay = 10,
	.prot = "rfcomm:0",
};

void
millisleep(time_t ms)
{
	if (ms <= 0)
		return;

	struct timespec tp = {
		.tv_sec = ms / 1000,
		.tv_nsec = (ms % 1000) * 1000000,
	};
	clock_nanosleep(CLOCK_MONOTONIC, 0, &tp, NULL);
}

void
usage(void)
{
	fprintf(stderr, "usage: [options] server\n");
	fprintf(stderr, "                 client addr\n");
	fprintf(stderr, "  -b  buffer size (default: %zu)\n", opt.bufsz);
	fprintf(stderr, "  -d  delay in ms (default: %jd)\n", (intmax_t)opt.delay);
	fprintf(stderr, "  -h  show this message\n");
	fprintf(stderr, "  -p  protocol (default: \"%s\")\n", opt.prot);
	fprintf(stderr, "  -r  retry on failure\n");
	fprintf(stderr, "\navailable mode: server, client\n");
	fprintf(stderr, "\navailable protocol:\n\"rfcomm:port\"\n\"l2cap:port:chan\"\n");
	exit(2);
}

void
server(int sfd, char *buf)
{
	struct sockaddr_l2 l2addr = { 0 }, rl2addr = { 0 };
	struct sockaddr_rc rfaddr = { 0 }, rrfaddr = { 0 };
	void *saddr, *raddr;
	size_t saddrlen;
	socklen_t sopt;
	ssize_t nr;
	uintmax_t tnr, tot;
	int cfd;

	if (strstr(opt.prot, "rfcomm")) {
		saddr = &rfaddr;
		raddr = &rrfaddr;
		saddrlen = sizeof(rfaddr);
		sopt = saddrlen;
		if (opt.port <= 0)
			opt.port = 0;

		rfaddr.rc_family = AF_BLUETOOTH;
		rfaddr.rc_bdaddr = *BDADDR_ANY;
		rfaddr.rc_channel = opt.port;
	} else if (strstr(opt.prot, "l2cap")) {
		saddr = &l2addr;
		raddr = &rl2addr;
		saddrlen = sizeof(l2addr);
		sopt = saddrlen;
		if (opt.port <= 0)
			opt.port = 0x1001;

		l2addr.l2_family = AF_BLUETOOTH;
		l2addr.l2_bdaddr = *BDADDR_ANY;
		if (strstr(opt.prot, "cid"))
			l2addr.l2_cid = htobs(opt.port);
		else
			l2addr.l2_psm = htobs(opt.port);
	} else
		assert(0);

	if (bind(sfd, saddr, saddrlen) < 0)
		err(1, "bind");

	if (listen(sfd, 1) < 0)
		err(1, "listen");

	printf("accepting connections using %s\n", opt.prot);

	cfd = accept(sfd, raddr, &sopt);
	if (cfd < 0)
		err(1, "accept");

	printf("accepted a connection\n");

	tnr = tot = 0;
	for (;;) {
		nr = recv(cfd, buf, opt.bufsz, 0);
		if (nr <= 0) {
			printf("%s\n", strerror(errno));
			if (opt.retry)
				continue;
			break;
		}
		tnr += nr;
		tot += opt.bufsz;
		printf("%zd %ju %ju %f\n", nr, tnr, tot, tnr * 1.0 / tot);
		millisleep(opt.delay);
	}

	close(cfd);
	close(sfd);
}

void
client(int sfd, const char *addr, char *buf)
{
	struct sockaddr_l2 l2addr = { 0 };
	struct sockaddr_rc rfaddr = { 0 };
	void *saddr;
	size_t saddrlen;
	ssize_t nw;
	uintmax_t tnw, tot;

	if (strstr(opt.prot, "rfcomm")) {
		str2ba(addr, &rfaddr.rc_bdaddr);
		saddr = &rfaddr;
		saddrlen = sizeof(rfaddr);
		if (opt.port <= 0)
			opt.port = 1;

		rfaddr.rc_family = AF_BLUETOOTH;
		rfaddr.rc_channel = opt.port;
	} else if (strstr(opt.prot, "l2cap")) {
		str2ba(addr, &l2addr.l2_bdaddr);
		saddr = &l2addr;
		saddrlen = sizeof(l2addr);
		if (opt.port <= 0)
			opt.port = 0x1001;

		l2addr.l2_family = AF_BLUETOOTH;
		if (strstr(opt.prot, "cid"))
			l2addr.l2_cid = htobs(opt.port);
		else
			l2addr.l2_psm = htobs(opt.port);
	} else
		assert(0);

	rfaddr.rc_channel = opt.port;

	printf("trying to connect to %s using %s\n", addr, opt.prot);
	if (connect(sfd, saddr, saddrlen) < 0)
		err(1, "connect");

	tnw = tot = 0;
	for (;;) {
		nw = write(sfd, buf, opt.bufsz);
		if (nw < 0) {
			printf("%s\n", strerror(errno));
			if (opt.retry)
				continue;
			break;
		}
		tnw += nw;
		tot += opt.bufsz;
		printf("%zd %ju %ju %f\n", nw, tnw, tot, tnw * 1.0 / tot);
		millisleep(opt.delay);
	}
	close(sfd);
}

int
main(int argc, char *argv[])
{
	int c, s;
	char *buf;

	while ((c = getopt(argc, argv, "b:d:hp:r")) != -1) {
		switch (c) {
		case 'b':
			sscanf(optarg, "%zu", &opt.bufsz);
			break;
		case 'd':
			opt.delay = atol(optarg);
			break;
		case 'p':
			snprintf(opt.prot, sizeof(opt.prot), "%s", optarg);
			break;
		case 'r':
			opt.retry = 1;
			break;
		case 'h':
		default:
			usage();
			break;
		}
	}
	argc -= optind;
	argv += optind;

	if (argc < 1)
		usage();

	if (sscanf(opt.prot, "rfcomm:%i", &opt.port) == 1)
		s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
	else if (sscanf(opt.prot, "l2cap:%i", &opt.port) == 1)
		s = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
	else
		err(1, "unknown protocol %s", opt.prot);

	if (s < 0)
		err(1, "socket");

	assert((buf = calloc(opt.bufsz, sizeof(*buf))));
	if (!strcmp(argv[0], "server"))
		server(s, buf);
	else if (!strcmp(argv[0], "client")) {
		if (argc < 2)
			usage();
		client(s, argv[1], buf);
	} else
		usage();
	free(buf);

	return 0;
}
