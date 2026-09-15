/*

Example of using the ISR defer API.
The ISR defer API allows one to schedule jobs to run inside an ISR that is executed later in a task context.
The API can also be used outside of a ISR context to just schedule random jobs as the example illustrates.

*/

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <hwif/util/vxbIsrDeferLib.h>

typedef struct {
	ISR_DEFER_JOB job;
	volatile atomic_int done;
	char sval[32];
} Context;

void
isrjob(void *arg)
{
	Context *c;

	c = arg;
	printf("%s(arg=%s)\n", __func__, c->sval);
	c->done = 1;
}

void
isrdefertest(void)
{
	ISR_DEFER_QUEUE_ID qid;
	Context ctx[8];
	size_t i;

	memset(ctx, 0, sizeof(ctx));
	for (i = 0; i < NELEMENTS(ctx); i++) {
		ctx[i].job.func = isrjob;
		ctx[i].job.pData = &ctx[i];
		sprintf(ctx[i].sval, "job #%zu", i);
	}

	qid = isrDeferQueueGet(NULL, 0, 0, 0);
	if (qid == NULL) {
		printf("Failed to create defer queue\n");
		return;
	}

	for (i = 0; i < NELEMENTS(ctx); i++) {
		isrDeferJobAdd(qid, &ctx[i].job);
		while (!ctx[i].done)
			sleep(1);
	}
}
