#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>
#include <vxWorks.h>
#include <taskLib.h>
#include <sysLib.h>
#include <msgQLib.h>

enum {
	MSGQLEN = 16,
	MSGSIZE = 256,
};

int
idler(_Vx_usr_arg_t arg1, _Vx_usr_arg_t arg2, _Vx_usr_arg_t arg3)
{
	MSG_Q_ID qid;
	_Vx_ticks_t timeout;
	char buf[MSGSIZE];
	int id;
	int r;

	id = arg1;
	qid = (MSG_Q_ID)arg2;
	timeout = arg3 * sysClkRateGet();
	for (;;) {
		r = msgQReceive(qid, buf, sizeof(buf), timeout);
		if (r >= 0)
			syslog(LOG_INFO, "%d: got %d bytes", id, r);
		else
			syslog(LOG_INFO, "%d: %s", id, strerror(errno));
	}
	return 0;
}

int
main(int argc, char *argv[])
{
	TASK_ID *tid;
	MSG_Q_ID *qid;
	char name[32];
	char buf[MSGSIZE];
	unsigned timeout;
	int prio, opts;
	int ntasks;
	int i;

	prio = 50;
	opts = 0;

	ntasks = 10;
	timeout = 2;
	if (argc >= 2)
		ntasks = atoi(argv[1]);
	if (argc >= 3)
		timeout = atoi(argv[2]);
	assert(ntasks >= 1);
	assert(timeout >= 1);

	tid = calloc(ntasks, sizeof(*tid));
	qid = calloc(ntasks, sizeof(*qid));
	assert(qid != NULL && tid != NULL);

	memset(buf, 0, MSGSIZE);
	for (i = 0; i < ntasks; i++) {
		qid[i] = msgQCreate(MSGQLEN, MSGSIZE, MSG_Q_FIFO);
		assert(qid[i] != MSG_Q_ID_NULL);
		msgQSend(qid[i], buf, MSGSIZE, WAIT_FOREVER, MSG_PRI_NORMAL);
	}

	printf("Starting %d tasks\n", ntasks);
	for (i = 0; i < ntasks; i++) {
		snprintf(name, sizeof(name), "idler-%d", i);
		tid[i] = taskOpen(name, prio, opts, OM_CREATE | OM_EXCL, NULL, 16384, NULL, (FUNCPTR)idler,
		                  i, (_Vx_usr_arg_t)qid[i], (i & 1) ? timeout : WAIT_FOREVER, 0, 0, 0, 0, 0, 0, 0);
		assert(tid != TASK_ID_NULL);

		assert(taskActivate(tid[i]) == OK);
	}
	for (i = 0; i < ntasks; i++)
		taskWait(tid[i], WAIT_FOREVER);

	return 0;
}
