#define _GNU_SOURCE
#include <stdbool.h>
#include <stdnoreturn.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <getopt.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/un.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <syslog.h>

typedef struct Conf Conf;

struct Conf {
	char *file;
	char *homedir;
	char options[64][256];
	bool daemonize;
	bool syslog;
	int timeout;
	int maxreqsz;
	int port;
	int maxcon;
	int gid;
	int uid;
} conf = {
	.file = "fingerd.conf",
	.homedir = "/tmp/fingerd",
	.daemonize = false,
	.syslog = true,
	.timeout = 15,
	.maxreqsz = 512,
	.port = 79,
	.maxcon = 512,
	.gid = -1,
	.uid = -1,
};

#define nelem(x) (sizeof(x) / sizeof(x[0]))
#define min(a, b) (((a) < (b)) ? (a) : (b))

noreturn void xerr(char *, ...);

char *
estrdup(char *s)
{
	if (!(s = strdup(s)))
		xerr("strdup: %s", strerror_l(errno, 0));
	return s;
}

bool
parsebool(char *val)
{
	if (!strcasecmp(val, "true"))
		return true;
	if (!strcasecmp(val, "false") || !strcasecmp(val, "") || !strcasecmp(val, "0"))
		return false;
	return true;
}

char *
trim(char *s)
{
	if (!s)
		return s;

	while (isspace(*s))
		s++;

	size_t n = strlen(s);
	while (n > 0 && isspace(s[n - 1]))
		n--;
	s[n] = '\0';

	return s;
}

void
parseopt(Conf *conf, char *line)
{
	char *saveptr;
	char *sline = strdup(line);
	char *key = trim(strtok_r(line, "=", &saveptr));
	char *val = trim(strtok_r(NULL, "=", &saveptr));

	if (!key || !val) {
		fprintf(stderr, "invalid option '%s'\n", sline);
		free(sline);
		return;
	}

	if (!strcasecmp(key, "daemonize"))
		conf->daemonize = parsebool(val);
	else if (!strcasecmp(key, "use_syslog"))
		conf->syslog = parsebool(val);
	else if (!strcasecmp(key, "server_port"))
		conf->port = atoi(val);
	else if (!strcasecmp(key, "max_connections"))
		conf->maxcon = atoi(val);
	else if (!strcasecmp(key, "homedir"))
		conf->homedir = estrdup(val);
	else if (!strcasecmp(key, "timeout"))
		conf->timeout = atoi(val);
	else if (!strcasecmp(key, "max_request_size"))
		conf->maxreqsz = atoi(val);
	else if (!strcasecmp(key, "group_id"))
		conf->gid = atoi(val);
	else if (!strcasecmp(key, "user_id"))
		conf->uid = atoi(val);
	else
		fprintf(stderr, "invalid option '%s'\n", sline);

	free(sline);

	if (conf->port < 0)
		xerr("invalid port");

	if (conf->maxcon < 0)
		xerr("invalid max connections");

	if (conf->timeout < 0)
		xerr("invalid timeout");

	if (conf->maxreqsz < 512 || conf->maxreqsz > 1024 * 1024)
		xerr("invalid max request size");
}

int
loadconf(Conf *conf, char *name)
{
	FILE *fp = fopen(name, "rt");
	if (!fp)
		return -1;

	char line[1024];
	while (fgets(line, sizeof(line), fp))
		parseopt(conf, line);

	fclose(fp);
	return 0;
}

noreturn void
usage(void)
{
	fprintf(stderr, "usage: fingerd [-ooption=value ...] [config]\n");
	fprintf(stderr, "  -h, --help                  show this message\n");
	fprintf(stderr, "  -o, --option option=value   set option\n");
	exit(2);
}

void
parseargs(int *argc, char ***argv)
{
	static struct option options[] = {
		{ "option", required_argument, 0, 0 },
		{ "help", no_argument, 0, 0 },
		{ 0, 0, 0, 0 },
	};
	int index;
	size_t opt = 0;

	int c;
	while ((c = getopt_long(*argc, *argv, "o:h", options, &index)) != -1) {
		switch (c) {
		case 0:
			if (index == 0)
				goto addoption;
			if (index == 1)
				usage();
			break;

		case 'o':
		addoption:
			if (opt >= nelem(conf.options))
				xerr("max number of options exceeded");
			snprintf(conf.options[opt], sizeof(conf.options[opt]), "%s", optarg);
			opt++;
			break;

		case 'h':
		default:
			usage();
		}
	}

	*argc -= optind;
	*argv += optind;

	if (*argc >= 1)
		conf.file = *argv[0];

	loadconf(&conf, conf.file);
	for (size_t i = 0; conf.options[i][0] != '\0'; i++)
		parseopt(&conf, conf.options[i]);
}

void
xmsg(FILE *fp, char *fmt, va_list ap)
{
	char str[1024];
	vsnprintf(str, sizeof(str), fmt, ap);

	if (conf.syslog)
		syslog(LOG_DAEMON, "%s", str);

	time_t now = time(NULL);
	fprintf(fp, "%s\t%s\n", ctime(&now), str);
}

noreturn void
xerr(char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	xmsg(stderr, fmt, ap);
	va_end(ap);
	exit(1);
}

void
xlog(char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	xmsg(stdout, fmt, ap);
	va_end(ap);
}

int
xlisten(int port)
{
	char serv[32];
	snprintf(serv, sizeof(serv), "%d", port);

	struct addrinfo hints = {
		.ai_flags = AI_PASSIVE,
		.ai_family = AF_UNSPEC,
		.ai_socktype = SOCK_STREAM,
	};
	struct addrinfo *res;
	int rv;
	if ((rv = getaddrinfo(NULL, serv, &hints, &res)) != 0)
		xerr("getaddrinfo: %s", gai_strerror(rv));

	struct addrinfo *ressave = res;
	int fd = -1;
	for (; res; res = res->ai_next) {
		fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (fd < 0)
			continue;

		int on = 1;
		setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

		if (bind(fd, res->ai_addr, res->ai_addrlen) == 0)
			break;

		close(fd);
		fd = -1;
	}
	freeaddrinfo(ressave);

	if (fd < 0)
		xerr("bind: failed to bind port %s", serv);

	if (listen(fd, 1024) < 0)
		xerr("listen: %s", strerror_l(errno, 0));

	return fd;
}

ssize_t
readreq(int fd, char *req, size_t size)
{
	memset(req, 0, size);

	char buf[512];
	ssize_t n;
	for (n = 0; size > 0;) {
		ssize_t nr = read(fd, buf, min(size - n, sizeof(buf) - 1));
		if (nr < 0) {
			if (errno == EINTR)
				continue;

			return -1;
		}
		buf[nr] = '\0';

		char *p = strstr(buf, "\r\n");
		if (p)
			nr = p - buf;

		memmove(req, buf, nr);
		req += nr;
		n += nr;

		if (p)
			break;
	}

	return n;
}

void
cow(int fd)
{
	dprintf(fd, "  \\ ^__^\n");
	dprintf(fd, "    (oo)\\_______\n");
	dprintf(fd, "    (__)\\       )\\/\\\n");
	dprintf(fd, "        ||----w |\n");
	dprintf(fd, "        ||     ||\r\n");
}

bool
validuser(char *s)
{
	if (!isalpha(s[0]) && s[0] != '_')
		return false;

	for (s++; s[0]; s++)
		if (!isalnum(s[0]) && s[0] != '_')
			return false;

	return true;
}

void
doreq(int fd, char *buf, size_t size)
{
	while (!strncmp(buf, "/W", 2)) {
		buf += 2;
		size -= 2;
	}

	if (size == 0) {
		dprintf(fd, "invalid request\r\n");
		return;
	}

	if (size > (size_t)conf.maxreqsz) {
		dprintf(fd, "request too big\r\n");
		return;
	}

	if (strchr(buf, '@')) {
		dprintf(fd, "finger forwarding not supported\r\n");
		return;
	}

	if (!validuser(buf)) {
		cow(fd);
		return;
	}

	char *file = NULL;
	if (asprintf(&file, "%s/%s/plan", conf.homedir, buf) < 0) {
		cow(fd);
		return;
	}

	FILE *fp = fopen(file, "rt");
	if (!fp) {
		cow(fd);
		goto out;
	}

	char line[32768];
	while (fgets(line, sizeof(line), fp)) {
		char *p = strstr(line, "\n");
		if (p) {
			if (p > line && p[-1] == '\r')
				p[-1] = '\n';

			p[0] = '\0';
		}
		dprintf(fd, "%s", line);
	}
	dprintf(fd, "\r\n");

out:
	free(file);
	if (fp)
		fclose(fp);
}

void
finger(int fd, char *rhost)
{
	struct timeval tv = {
		.tv_sec = conf.timeout,
	};
	if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
		xlog("%s: setsockopt: %s", rhost, strerror_l(errno, 0));
		return;
	}

	static char sbuf[8192];
	char *buf = sbuf;
	size_t size = conf.maxreqsz * 2;
	if (nelem(sbuf) < size) {
		if (!(buf = malloc(size))) {
			xlog("%s: malloc: %s", rhost, strerror_l(errno, 0));
			return;
		}
	}

	ssize_t n = readreq(fd, buf, size);
	if (n > 0)
		doreq(fd, buf, n);
	else
		xlog("%s: read: %s", rhost, strerror_l(errno, 0));

	if (buf != sbuf)
		free(buf);
}

void
droppriv(void)
{
	if (conf.gid > 0 && setgid(conf.gid) != 0)
		xerr("setgid: %s", strerror_l(errno, 0));
	if (conf.uid > 0 && setuid(conf.uid) != 0)
		xerr("setuid: %s", strerror_l(errno, 0));
}

void
serve(void)
{
	xlog("fingerd daemon starting...");

	int cons = 0;
	int lfd = xlisten(conf.port);

	droppriv();
	for (;;) {
		while (cons > 0) {
			pid_t pid = waitpid(-1, NULL, WNOHANG);
			if (pid > 0) {
				cons--;
				continue;
			}

			if (cons >= conf.maxcon) {
				wait(NULL);
				cons--;
			}

			break;
		}

		struct sockaddr addr;
		socklen_t addrlen = sizeof(addr);
		int fd;
		if ((fd = accept(lfd, &addr, &addrlen)) < 0) {
			xlog("accept: %s", strerror_l(errno, 0));
			continue;
		}

		char rhost[256];
		switch (addr.sa_family) {
		case AF_INET: {
			struct sockaddr_in *sa = (struct sockaddr_in *)&addr;
			inet_ntop(AF_INET, &sa->sin_addr, rhost, sizeof(rhost));
			break;
		}

		case AF_INET6: {
			struct sockaddr_in6 *sa = (struct sockaddr_in6 *)&addr;
			inet_ntop(AF_INET6, &sa->sin6_addr, rhost, sizeof(rhost));
			break;
		}

		case AF_UNIX: {
			struct sockaddr_un *sa = (struct sockaddr_un *)&addr;
			snprintf(rhost, sizeof(rhost), "%s", sa->sun_path);
			break;
		}

		default:
			snprintf(rhost, sizeof(rhost), "unknown");
			break;
		}

		xlog("accept: %s", rhost);

		pid_t pid = fork();
		if (pid < 0) {
			xlog("fork: %s", strerror_l(errno, 0));
			close(fd);
			continue;
		}

		if (pid) {
			close(fd);
			cons++;
		} else {
			close(lfd);
			finger(fd, rhost);
			close(fd);
			exit(0);
		}
	}
}

int
main(int argc, char *argv[])
{
	parseargs(&argc, &argv);
	if (conf.daemonize)
		daemon(0, 0);
	if (conf.syslog)
		openlog("fingerd", 0, LOG_DAEMON);
	serve();
	return 0;
}
