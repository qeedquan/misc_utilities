#include <vxWorks.h>
#include <iosLib.h>
#include <evdevLib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

int
std_task(void)
{
	int i;
	char buf[32];

	while (fgets(buf, sizeof(buf), stdin))
		printf("%s", buf);

	for (i = 0; i <= 5; i++) {
		printf("hello %d\n", i);
		fprintf(stderr, "world %d\n", i);
	}
	fflush(stdout);
	fflush(stderr);
	return OK;
}

STATUS
redirect_std(void)
{
	TASK_ID tid;
	char buf[32];
	int ifd, ofd, efd;
	int tifd, tofd, tefd;
	int len;
	int i;

	ifd = open("/ram0/stdin.txt", O_RDWR | O_CREAT, 0777);
	ofd = open("/ram0/stdout.txt", O_RDWR | O_CREAT, 0777);
	efd = open("/ram0/stderr.txt", O_RDWR | O_CREAT, 0777);
	for (i = 0; i < 10; i++) {
		len = snprintf(buf, sizeof(buf), "in %d\n", i);
		write(ifd, buf, len);
	}
	close(ifd);
	ifd = open("/ram0/stdin.txt", O_RDWR | O_CREAT, 0777);

	tid = taskCreate("Redirection Task", 100, VX_FP_TASK, 0x4000, std_task, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	tifd = ioTaskStdGet(tid, STD_IN);
	tofd = ioTaskStdGet(tid, STD_OUT);
	tefd = ioTaskStdGet(tid, STD_ERR);
	ioTaskStdSet(tid, STD_IN, ifd);
	ioTaskStdSet(tid, STD_OUT, ofd);
	ioTaskStdSet(tid, STD_ERR, efd);

	taskActivate(tid);
	taskWait(tid, WAIT_FOREVER);

	ioTaskStdSet(tid, STD_IN, tifd);
	ioTaskStdSet(tid, STD_OUT, tofd);
	ioTaskStdSet(tid, STD_ERR, tefd);

	close(ifd);
	close(ofd);
	close(efd);

	return OK;
}
