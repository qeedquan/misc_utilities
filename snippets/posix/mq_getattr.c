#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mqueue.h>
#include <err.h>

int
main(int argc, char **argv)
{
	struct mq_attr attr;
	mqd_t mq;

	if (argc < 2) {
		fprintf(stderr, "usage: file\n");
		exit(2);
	}

	mq = mq_open(argv[1], O_RDONLY);
	if (mq < 0)
		err(1, "mq_open: %s", argv[1]);

	if (mq_getattr(mq, &attr) < 0)
		err(1, "mq_getattr");

	printf("flags: %ld\n", attr.mq_flags);
	printf("maxmsg: %ld\n", attr.mq_maxmsg);
	printf("msgsize: %ld\n", attr.mq_msgsize);
	printf("curmsgs: %ld\n", attr.mq_curmsgs);

	mq_close(mq);

	return 0;
}
