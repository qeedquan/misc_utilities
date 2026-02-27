#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mqueue.h>
#include <unistd.h>
#include <err.h>

int
main(int argc, char **argv)
{
	struct mq_attr attr;
	mqd_t mq;

	if (argc < 2) {
		fprintf(stderr, "usage: file [max_messages max_size]\n");
		exit(2);
	}

	memset(&attr, 0, sizeof(attr));
	if (argc == 4) {
		attr.mq_maxmsg = atoi(argv[2]);
		attr.mq_msgsize = atoi(argv[3]);
	}

	mq_unlink(argv[1]);
	if (attr.mq_maxmsg > 0)
		mq = mq_open(argv[1], O_RDWR | O_CREAT, 0755, &attr);
	else
		mq = mq_open(argv[1], O_RDWR | O_CREAT, 0755, NULL);

	if (mq < 0)
		err(1, "mq_open: %s", argv[1]);

	if (mq_getattr(mq, &attr) < 0)
		err(1, "mq_getattr");

	printf("flags: %ld\n", attr.mq_flags);
	printf("maxmsg: %ld\n", attr.mq_maxmsg);
	printf("msgsize: %ld\n", attr.mq_msgsize);
	printf("curmsgs: %ld\n", attr.mq_curmsgs);

	for (;;)
		sleep(1);

	mq_close(mq);

	return 0;
}
