// daytime server
// listens on a socket and write the date to client
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>

enum {
	MAXLINE = 4096,
	LISTENQ = 1024
};

void
usage(void)
{
	fprintf(stderr, "usage: daytimesrv [options]\n");
	fprintf(stderr, "  -h  show this message\n");
	fprintf(stderr, "  -p  port to listen on\n");
	exit(2);
}

int
main(int argc, char *argv[])
{
	int listenfd, connfd, port, c;
	struct sockaddr_in servaddr;
	char buf[MAXLINE];
	time_t ticks;

	port = 6000;
	while ((c = getopt(argc, argv, "p:h")) != -1) {
		switch (c) {
		case 'p':
			port = atoi(optarg);
			break;
		case 'h':
			usage();
		}
	}

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd < 0)
		err(1, "socket");

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);

	if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
		err(1, "bind");

	if (listen(listenfd, LISTENQ) < 0)
		err(1, "listen");

	printf("listening on :%d\n", port);
	for (;;) {
		connfd = accept(listenfd, NULL, NULL);
		if (connfd < 0) {
			perror("accept");
			continue;
		}

		printf("got a connection\n");
		ticks = time(NULL);
		snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));
		write(connfd, buf, strlen(buf));
		close(connfd);
	}

	return 0;
}
