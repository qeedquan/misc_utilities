#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <rtpLib.h>
#include <ioLib.h>
#include <vxWorks.h>

int
main(int argc, char *argv[])
{
	RTP_DESC rd;
	char path[VX_RTP_NAME_LENGTH + 1];
	long long rid;
	size_t nfds;
	int i;

	if (argc < 2) {
		fprintf(stderr, "usage: rtpid ...\n");
		return 2;
	}

	for (i = 1; i < argc; i++) {
		rid = atoll(argv[i]);
		if (vxeAbsPathGet(rid, path, sizeof(path)) != OK) {
			printf("Failed to get RTP %lld path: %s\n", rid, strerror(errno));
			continue;
		}
		if (rtpInfoGet(rid, &rd) != OK) {
			printf("Failed to get RTP %lld info: %s\n", rid, strerror(errno));
			continue;
		}

		nfds = rtpIoTableSizeGet(rid);

		printf("%lld: %s\n", rid, path);
		printf("Status:           %d\n", rd.status);
		printf("Options:          %#llx\n", (long long)rd.options);
		printf("Entry point:      %p\n", rd.entrAddr);
		printf("Initial task ID:  %lld\n", (long long)rd.initTaskId);
		printf("RTP ID of parent: %lld\n", (long long)rd.parentId);
		printf("Path name:        %s\n", rd.pathName);
		printf("Task count        %lld\n", (long long)rd.taskCnt);
		printf("Text start:       %p\n", rd.textStart);
		printf("Text end:         %p\n", rd.textEnd);
		printf("Max fds:          %zu\n", nfds);
	}

	return 0;
}
