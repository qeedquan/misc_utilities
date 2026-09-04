#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdnoreturn.h>
#include <errno.h>
#include <getopt.h>
#include <err.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include <sys/syscall.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>

typedef struct {
	int numthreads;
	int maxcon;
	int port;
} Option;

typedef struct {
	int lfd;
	int ufd;
	atomic_int cons;
} Context;

Option opt = {
	.numthreads = 32,
	.maxcon = 1024,
	.port = 10009,
};

Context ctx;

void *
xcalloc(size_t nmemb, size_t size)
{
	void *ptr;

	ptr = calloc(nmemb, size);
	if (!ptr)
		abort();
	return ptr;
}

void
xpthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg)
{
	if (pthread_create(thread, attr, start_routine, arg) < 0)
		errx(1, "pthread_create: %s", strerror(errno));
}

int
xepoll_create1(int flags)
{
	int fd;

	fd = epoll_create1(flags);
	if (fd < 0)
		errx(1, "epoll_create1: %s", strerror(errno));
	return fd;
}

void
xepoll_ctl(int epfd, int op, int fd, struct epoll_event *event)
{
	int rv;

	rv = epoll_ctl(epfd, op, fd, event);
	if (rv < 0)
		errx(1, "epoll_ctl: %s", strerror(errno));
}

int
xepoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout)
{
	int rv;

	rv = epoll_wait(epfd, events, maxevents, timeout);
	if (rv < 0)
		errx(1, "epoll_wait: %s", strerror(errno));
	return rv;
}

void
nonblocking(int fd)
{
	int flags;

	flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		errx(1, "fcntl: %s", strerror(errno));

	flags |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, flags) == -1)
		errx(1, "fcntl: %s", strerror(errno));
}

int
makesocket(int port, bool tcp)
{
	struct addrinfo hints, *res, *ressave;
	char serv[32];
	int fd, on, rv;

	snprintf(serv, sizeof(serv), "%d", port);
	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = (tcp) ? SOCK_STREAM : SOCK_DGRAM;

	rv = getaddrinfo(NULL, serv, &hints, &res);
	if (rv != 0)
		errx(1, "getaddrinfo: %s", gai_strerror(rv));

	fd = -1;
	for (ressave = res; res; res = res->ai_next) {
		fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (fd < 0)
			continue;

		if (tcp) {
			on = 1;
			setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
			setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
		}

		if (bind(fd, res->ai_addr, res->ai_addrlen) == 0)
			break;

		close(fd);
		fd = -1;
	}
	freeaddrinfo(ressave);

	if (fd < 0)
		errx(1, "bind: failed to bind port %s", serv);

	if (tcp) {
		nonblocking(fd);
		if (listen(fd, opt.maxcon) < 0)
			errx(1, "listen: %s", strerror(errno));
	}
	return fd;
}

char *
addr2str(struct sockaddr *addr, char *buf, size_t len)
{
	switch (addr->sa_family) {
	case AF_INET: {
		struct sockaddr_in *sa = (struct sockaddr_in *)addr;
		inet_ntop(AF_INET, &sa->sin_addr, buf, len);
		break;
	}

	case AF_INET6: {
		struct sockaddr_in6 *sa = (struct sockaddr_in6 *)addr;
		inet_ntop(AF_INET, &sa->sin6_addr, buf, len);
		break;
	}

	case AF_UNIX: {
		struct sockaddr_un *sa = (struct sockaddr_un *)addr;
		snprintf(buf, len, "%s", sa->sun_path);
		break;
	}

	default:
		snprintf(buf, len, "unknown host");
		break;
	}
	return buf;
}

noreturn void
usage(void)
{
	fprintf(stderr, "usage: [-hmpt]\n");
	fprintf(stderr, "  -h                show this message\n");
	fprintf(stderr, "  -m <num_conns>    specify max number of connections (default %d)\n", opt.maxcon);
	fprintf(stderr, "  -p <port> listen  on port (default %d)\n", opt.port);
	fprintf(stderr, "  -t <num_threads>  number of threads to spawn for running (default %d)\n", opt.numthreads);
	exit(2);
}

void
parseargs(int *argc, char ***argv)
{
	int c;

	while ((c = getopt(*argc, *argv, "hm:p:t:")) != -1) {
		switch (c) {
		case 'h':
			usage();
			break;

		case 'm':
			opt.maxcon = atoi(optarg);
			break;

		case 'p':
			opt.port = atoi(optarg);
			break;

		case 't':
			opt.numthreads = atoi(optarg);
			break;
		}
	}

	*argc -= optind;
	*argv += optind;
}

void
initctx(void)
{
	ctx.lfd = makesocket(opt.port, true);
	ctx.ufd = makesocket(opt.port, false);
}

void *
tcpdiscard(void *arg)
{
	pid_t tid;
	struct epoll_event ev, *events;
	struct sockaddr addr;
	socklen_t addrlen;
	char buf[32768], host[256];
	int epfd, lfd, sfd, nev, i;
	ssize_t nr;

	tid = syscall(SYS_gettid);
	printf("tcp(%d): listening on %d\n", tid, opt.port);

	epfd = xepoll_create1(0);
	events = xcalloc(opt.maxcon + 1, sizeof(*events));

	lfd = ctx.lfd;
	memset(&ev, 0, sizeof(ev));
	ev.data.fd = lfd;
	ev.events = EPOLLIN | EPOLLEXCLUSIVE;
	xepoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);

	for (;;) {
		nev = xepoll_wait(epfd, events, opt.maxcon + 1, -1);
		for (i = 0; i < nev; i++) {
			printf("tcp(%d): ", tid);
			if (events[i].data.fd == lfd) {
				sfd = accept(lfd, &addr, &addrlen);
				if (sfd < 0) {
					printf("accept: %s\n", strerror(errno));
					continue;
				}

				ev.data.fd = sfd;
				xepoll_ctl(epfd, EPOLL_CTL_ADD, sfd, &ev);
				atomic_fetch_add(&ctx.cons, 1);
				printf("connection from %s (%d connections total)\n", addr2str(&addr, host, sizeof(host)), atomic_load(&ctx.cons));
			} else {
				nr = recvfrom(events[i].data.fd, buf, sizeof(buf), 0, &addr, &addrlen);
				if (nr < 0)
					printf("%s\n", strerror(errno));
				else if (nr == 0) {
					xepoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, &events[i]);
					close(events[i].data.fd);
					atomic_fetch_add(&ctx.cons, -1);
					printf("closed connection from %s (%d connections total)\n", addr2str(&addr, host, sizeof(host)), atomic_load(&ctx.cons));
				} else
					printf("discarded %zd bytes from %s\n", nr, addr2str(&addr, host, sizeof(host)));
			}
		}
	}

	(void)arg;

	return NULL;
}

void *
udpdiscard(void *arg)
{
	pid_t tid;
	char buf[32768], host[256];
	struct sockaddr addr;
	socklen_t addrlen;
	ssize_t nr;
	int fd;

	tid = syscall(SYS_gettid);
	printf("udp(%d): listening on %d\n", tid, opt.port);

	fd = ctx.ufd;
	for (;;) {
		nr = recvfrom(fd, buf, sizeof(buf), 0, &addr, &addrlen);
		printf("udp(%d): ", tid);
		if (nr < 0)
			printf("%s\n", strerror(errno));
		else
			printf("discarded %zd bytes from %s\n", nr, addr2str(&addr, host, sizeof(host)));
	}

	(void)arg;

	return NULL;
}

int
main(int argc, char *argv[])
{
	pthread_t *tcpthreads, *udpthreads;
	int i;

	parseargs(&argc, &argv);

	initctx();
	tcpthreads = xcalloc(opt.numthreads, sizeof(*tcpthreads));
	udpthreads = xcalloc(opt.numthreads, sizeof(*udpthreads));
	for (i = 0; i < opt.numthreads; i++) {
		xpthread_create(&tcpthreads[i], NULL, tcpdiscard, NULL);
		xpthread_create(&udpthreads[i], NULL, udpdiscard, NULL);
	}
	for (i = 0; i < opt.numthreads; i++) {
		pthread_join(tcpthreads[i], NULL);
		pthread_join(udpthreads[i], NULL);
	}
	return 0;
}
