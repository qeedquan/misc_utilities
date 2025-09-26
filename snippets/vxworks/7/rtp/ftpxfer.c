#include <vxWorks.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <libgen.h>
#include <err.h>
#include <ftpLib.h>

typedef struct {
	char host[256];
	char user[256];
	char acct[256];
	char pass[256];
	char file[256];
	char output[256];
} Conf;

Conf conf;

void
usage(void)
{
	fprintf(stderr, "usage: [options] file\n");
	fprintf(stderr, "  -a    specify account\n");
	fprintf(stderr, "  -h    show this message\n");
	fprintf(stderr, "  -H    specify host\n");
	fprintf(stderr, "  -o    specify output file\n");
	fprintf(stderr, "  -p    specify password\n");
	fprintf(stderr, "  -u    specify user\n");
	exit(2);
}

void
parseargs(int *argc, char ***argv)
{
	int c;

	while ((c = getopt(*argc, *argv, "a:hH:o:p:u:")) != -1) {
		switch (c) {
		case 'a':
			snprintf(conf.acct, sizeof(conf.acct), "%s", optarg);
			break;

		case 'h':
			usage();
			break;

		case 'H':
			snprintf(conf.host, sizeof(conf.host), "%s", optarg);
			break;

		case 'o':
			snprintf(conf.output, sizeof(conf.output), "%s", optarg);
			break;

		case 'p':
			snprintf(conf.pass, sizeof(conf.pass), "%s", optarg);
			break;

		case 'u':
			snprintf(conf.user, sizeof(conf.user), "%s", optarg);
			break;
		}
	}

	*argc -= optind;
	*argv += optind;

	if (*argc < 1)
		usage();

	snprintf(conf.file, sizeof(conf.file), "%s", *argv[0]);
	if (conf.output[0] == '\0')
		snprintf(conf.output, sizeof(conf.output), "%s", conf.file);
}

int
main(int argc, char *argv[])
{
	char dir[256];
	char base[256];
	char buf[8192];
	FILE *fp;
	ssize_t nr;
	int ctrl;
	int data;

	parseargs(&argc, &argv);

	snprintf(dir, sizeof(dir), "%s", conf.file);
	snprintf(base, sizeof(base), "%s", conf.file);

	if (ftpXfer(conf.host, conf.user, conf.pass, conf.acct, "RETR %s", dirname(dir), basename(base), &ctrl, &data) == ERROR)
		errx(1, "Failed to get file");

	fp = fopen(conf.output, "wb");
	if (!fp)
		errx(1, "Failed to create file for writing: %s", strerror(errno));

	while ((nr = read(data, buf, sizeof(buf))) > 0)
		fwrite(buf, nr, 1, fp);

	if (nr < 0)
		errx(1, "Error reading file: %s", strerror(errno));

	if (ftpReplyGet(ctrl, TRUE) != FTP_COMPLETE)
		errx(1, "Failed to get ftp reply");

	if (ftpCommand(ctrl, "QUIT", 0, 0, 0, 0, 0, 0) != FTP_COMPLETE)
		errx(1, "Failed to close ftp connection");

	fclose(fp);
	close(data);
	close(ctrl);

	return 0;
}
