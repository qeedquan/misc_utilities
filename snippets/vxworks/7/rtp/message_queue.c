#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <vxWorks.h>
#include <taskLib.h>
#include <msgQLib.h>
#include <sysLib.h>
#include <semLib.h>
#include "util.h"

enum {
	MSG_INCREMENT = 1,
	MSG_ACK,
};

enum {
	QCNT = 10,
	QLEN = 1024,
};

typedef struct {
	TASK_ID mtid;
	TASK_ID ptid;
	TASK_ID *wtid;

	MSG_Q_ID mq;
	MSG_Q_ID pq;
	MSG_Q_ID *wq;

	unsigned *cnt;

	int nw;
} Context;

void
send(MSG_Q_ID qid, unsigned val)
{
	char buf[QLEN];
	int r;

	memcpy(buf, &val, sizeof(val));
	r = msgQSend(qid, buf, sizeof(buf), WAIT_FOREVER, MSG_PRI_NORMAL);
	if (r != OK)
		xprintf("Failed to send data: %s\n", strerror(errno));
}

unsigned
recv(MSG_Q_ID qid)
{
	char buf[QLEN];
	unsigned val;
	int r;

	val = 0;
	r = msgQReceive(qid, buf, sizeof(buf), WAIT_FOREVER);
	if (r != QLEN)
		xprintf("Failed to receive data: %s\n", strerror(errno));
	memcpy(&val, buf, sizeof(val));
	return val;
}

int
producer(_Vx_usr_arg_t arg1)
{
	Context *c;
	time_t t0, t1;
	int i;

	c = (void *)arg1;
	t0 = time(NULL);
	for (;;) {
		send(c->mq, MSG_INCREMENT);
		recv(c->pq);

		t1 = time(NULL);
		if (t1 != t0) {
			xprintf("%ld: New state:\n", t1);
			for (i = 0; i < c->nw; i++)
				xprintf("%u ", c->cnt[i]);
			xprintf("\n");

			t0 = t1;
		}
	}
	return 0;
}

int
manager(_Vx_usr_arg_t arg1)
{
	Context *c;
	unsigned val;
	int nmsg;
	int i;

	c = (void *)arg1;
	for (;;) {
		nmsg = msgQNumMsgs(c->mq);
		if (nmsg < 1) {
			taskDelay(nmsg);
			continue;
		}

		val = recv(c->mq);
		switch (val) {
		case MSG_INCREMENT:
			for (i = 0; i < c->nw; i++) {
				send(c->wq[i], MSG_INCREMENT);
				recv(c->mq);
			}
			send(c->pq, MSG_ACK);
			break;
		}
	}
	return 0;
}

int
worker(_Vx_usr_arg_t arg1, _Vx_usr_arg_t arg2)
{
	Context *c;
	int id;
	unsigned val;

	c = (void *)arg1;
	id = arg2;

	for (;;) {
		val = recv(c->wq[id]);
		switch (val) {
		case MSG_INCREMENT:
			c->cnt[id]++;
			send(c->mq, MSG_ACK);
			break;
		}
	}
	return 0;
}

Context *
newcontext(int nw)
{
	Context *c;
	char name[32];
	int prio, opts;
	int qcnt, qlen, qopt;
	int i;

	prio = 100;
	opts = 0;

	qcnt = QCNT;
	qlen = QLEN;
	qopt = MSG_Q_FIFO;

	c = calloc(1, sizeof(*c));
	assert(c != NULL);

	c->cnt = calloc(nw, sizeof(*c->cnt));
	assert(c->cnt != NULL);

	c->mq = msgQCreate(qcnt, qlen, qopt);
	c->pq = msgQCreate(qcnt, qlen, qopt);
	c->wq = calloc(nw, sizeof(*c->wq));
	assert(c->mq != MSG_Q_ID_NULL);
	assert(c->pq != MSG_Q_ID_NULL);
	assert(c->wq != NULL);
	for (i = 0; i < nw; i++) {
		c->wq[i] = msgQCreate(qcnt, qlen, qopt);
		assert(c->wq[i] != MSG_Q_ID_NULL);
	}

	c->mtid = taskOpen("manager", prio, opts, OM_CREATE | OM_EXCL, NULL, 128 * 1024, NULL, (FUNCPTR)manager, (_Vx_usr_arg_t)c, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	assert(c->mtid != TASK_ID_NULL);

	c->ptid = taskOpen("producer", prio, opts, OM_CREATE | OM_EXCL, NULL, 128 * 1024, NULL, (FUNCPTR)producer, (_Vx_usr_arg_t)c, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	assert(c->ptid != TASK_ID_NULL);

	c->wtid = calloc(nw, sizeof(*c->wtid));
	assert(c->wtid != NULL);
	for (i = 0; i < nw; i++) {
		snprintf(name, sizeof(name), "worker-%d", i);
		c->wtid[i] = taskOpen(name, prio, opts, OM_CREATE | OM_EXCL, NULL, 128 * 1024, NULL, (FUNCPTR)worker, (_Vx_usr_arg_t)c, i, 0, 0, 0, 0, 0, 0, 0, 0);
		assert(c->wtid[i] != TASK_ID_NULL);
	}

	c->nw = nw;
	return c;
}

void
runcontext(Context *c)
{
	int i;

	assert(taskActivate(c->mtid) == OK);
	assert(taskActivate(c->ptid) == OK);
	for (i = 0; i < c->nw; i++)
		assert(taskActivate(c->wtid[i]) == OK);
	taskWait(c->mtid, WAIT_FOREVER);
}

int
main(int argc, char *argv[])
{
	int nw;

	initutil();

	nw = 50;
	if (argc >= 2)
		nw = atoi(argv[1]);
	assert(nw >= 1);

	runcontext(newcontext(nw));
	return 0;
}
