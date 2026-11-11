#include "vxWorks.h"

typedef struct {
	struct mq_attr attr;
	mqd_t qid;
	char name[32];
} QD;

static pthread_mutex_t qdlk = PTHREAD_MUTEX_INITIALIZER;
static QD qds[256];
static size_t nqd;

static void
qdlock(void)
{
	pthread_mutex_lock(&qdlk);
}

static void
qdunlock(void)
{
	pthread_mutex_unlock(&qdlk);
}

static MSG_Q_ID
mkqueue(const char *name, size_t maxMsgs, size_t maxMsgLength, int mode)
{
	struct mq_attr attr;
	mqd_t qid;
	char ident[32], *prefix;
	int flag;
	QD *qd;

	memset(&attr, 0, sizeof(attr));
	attr.mq_maxmsg = min(maxMsgs, 10);
	attr.mq_msgsize = maxMsgLength;

	prefix = getenv("VX_MSGQ_PREFIX");
	if (!prefix)
		prefix = "vx";

	if (!name)
		name = "anon";

	flag = O_RDWR | O_CLOEXEC;
	if (mode & OM_CREATE)
		flag |= O_CREAT;
	if (mode & OM_EXCL)
		flag |= OM_EXCL;

	snprintf(ident, sizeof(ident), "/%s-%s-%zu", prefix, name, nqd);
	mq_unlink(ident);

	qid = mq_open(ident, flag, 0644, &attr);
	if (qid < 0) {
		eprintf("Failed to create queue name: %s: %s\n", ident, strerror(errno));
		return MSG_Q_ID_NULL;
	}

	eprintf("Created queue %s\n", ident);

	qd = &qds[++nqd];
	qd->qid = qid;
	snprintf(qd->name, sizeof(qd->name), "%s", ident);
	mq_getattr(qid, &qd->attr);

	return qd;
}

MSG_Q_ID
msgQCreate(size_t maxMsgs, size_t maxMsgLength, int options)
{
	MSG_Q_ID ret;

	eprintf("(maxMsgs=%zu, maxMsgLength=%zu, options=%#x)\n", maxMsgs, maxMsgLength, options);

	ret = MSG_Q_ID_NULL;
	qdlock();
	if (nqd >= nelem(qds))
		goto out;

	ret = mkqueue(NULL, maxMsgs, maxMsgLength, OM_CREATE);

out:
	qdunlock();
	return ret;
}

MSG_Q_ID
msgQOpen(const char *name, size_t maxMsgs, size_t maxMsgLength, int options, int mode, void *context)
{
	MSG_Q_ID ret;

	eprintf("(name=%s, maxMsgs=%zu, maxMsgLength=%zu, options=%#x, mode=%#x, context=%p)\n", name, maxMsgs, maxMsgLength, options, mode, context);

	ret = MSG_Q_ID_NULL;
	if ((options & MSG_Q_FIFO) != MSG_Q_FIFO)
		return ret;

	qdlock();
	if (nqd >= nelem(qds))
		goto out;

	ret = mkqueue(name, maxMsgs, maxMsgLength, mode);

out:
	qdunlock();
	return ret;
}

STATUS
msgQClose(MSG_Q_ID msgQId)
{
	QD *qd;
	const char *name;

	name = "(nil)";
	qd = msgQId;
	if (qd)
		name = qd->name;

	eprintf("(msgQId=%p, queue_name=%s)\n", msgQId, name);
	return OK;
}

STATUS
msgQSend(MSG_Q_ID msgQId, char *buffer, size_t nBytes, _Vx_ticks_t timeout, int priority)
{
	QD *qd;
	struct timespec tp;
	uint64_t ms;
	int r;

	qdlock();
	qd = msgQId;
	if (qd == NULL) {
		qdunlock();
		return ERROR;
	}
	qdunlock();

	eprintf("(msgQId=%d, queue_name=%s, buffer=%p, nBytes=%zu, timeout=%d priority=%d)\n",
	        msgQId, qd->name, buffer, nBytes, timeout, priority);

	if (timeout < 0)
		r = mq_send(qd->qid, buffer, nBytes, priority);
	else {
		clock_gettime(CLOCK_REALTIME, &tp);
		ms = tick_to_millisecond(timeout);

		tp.tv_sec += ms / 1000;
		tp.tv_nsec += (ms % 1000) * 1000000;

		r = mq_timedsend(qd->qid, buffer, nBytes, priority, &tp);
	}

	if (r < 0) {
		eprintf("Failed to send message %s\n", strerror(errno));
		return ERROR;
	}

	return OK;
}

ssize_t
msgQReceive(MSG_Q_ID msgQId, char *buffer, size_t maxNBytes, _Vx_ticks_t timeout)
{
	QD *qd;
	struct timespec tp;
	uint64_t ms;
	unsigned int prio;
	ssize_t nr;

	qdlock();
	qd = msgQId;
	if (qd == NULL) {
		qdunlock();
		return -1;
	}
	qdunlock();

	eprintf("(msgQId=%d, queue_name=%s, queue_max_size=%zu, buffer=%p, maxNBytes=%zu, timeout=%d)\n",
	        msgQId, qd->name, qd->attr.mq_msgsize, buffer, maxNBytes, timeout);

	if (timeout < 0)
		nr = mq_receive(qd->qid, buffer, maxNBytes, &prio);
	else {
		clock_gettime(CLOCK_REALTIME, &tp);
		ms = tick_to_millisecond(timeout);

		tp.tv_sec += ms / 1000;
		tp.tv_nsec += (ms % 1000) * 1000000;

		nr = mq_timedreceive(qd->qid, buffer, maxNBytes, &prio, &tp);
	}

	if (nr < 0) {
		eprintf("Failed to receive message (queue_name=%s) %d: %s\n", qd->name, errno, strerror(errno));
		if (errno == ETIMEDOUT)
			errno = S_objLib_OBJ_TIMEOUT;
	}

	return nr;
}

int
msgQNumMsgs(MSG_Q_ID msgQId)
{
	QD *qd;
	struct mq_attr attr;

	qdlock();
	qd = msgQId;
	if (qd == NULL) {
		qdunlock();
		return -1;
	}
	qdunlock();

	if (mq_getattr(qd->qid, &attr) < 0)
		return ERROR;

	return attr.mq_curmsgs;
}
