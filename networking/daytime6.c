// implements the daytime client for ipv6
// which prints out the date from the server
// when connected to it
// http://tf.nist.gov/tf-cgi/servers.cgi#
// has some servers to try this on
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <stdnoreturn.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>

enum
{
    MAXLINE = 4096
};

noreturn void
fatal(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    exit(1);
}

int
main(int argc, char *argv[])
{
    int fd, n, rc, flags;
    char line[MAXLINE];
    fd_set fdset;
    struct sockaddr_in6 addr;
    struct timeval tv;

    if (argc != 2)
        fatal("usage: <ip>");

    fd = socket(AF_INET6, SOCK_STREAM, 0);
    if (fd < 0)
        fatal("socket: %s", strerror(errno));

    memset(&addr, 0, sizeof(addr));
    addr.sin6_family = AF_INET6;
    // daytime port
    addr.sin6_port = htons(13);

    // convert textual form of ipv4/ipv6 to binary form
    if (inet_pton(AF_INET6, argv[1], &addr.sin6_addr) <= 0)
        fatal("inet_pton: invalid ip: %s", argv[1]);

    rc = connect(fd, (struct sockaddr*) &addr, sizeof(addr));
    if (rc < 0)
        fatal("connect: %s\n", strerror(errno));

    while ((n = read(fd, line, MAXLINE)) > 0) {
        line[n] = '\0';
        printf("%s", line);
    }
    if (n < 0)
        fatal("read: %s", strerror(errno));

    return 0;
}
