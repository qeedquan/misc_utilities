#ifndef _MSGQLIB_H_
#define _MSGQLIB_H_

#include <stdio.h>
#include "types/vxTypes.h"

typedef void *MSG_Q_ID;

#define MSG_Q_ID_NULL NULL

#define MSG_PRI_NORMAL 0
#define MSG_PRI_URGENT 1

#define MSG_Q_FIFO 0x0
#define MSG_Q_PRIORITY 0x01

#define S_msgQLib_INVALID_MSG_LENGTH (M_msgQLib | 1)
#define S_msgQLib_NON_ZERO_TIMEOUT_AT_INT_LEVEL (M_msgQLib | 2)
#define S_msgQLib_INVALID_QUEUE_TYPE (M_msgQLib | 3)
#define S_msgQLib_ILLEGAL_OPTIONS (M_msgQLib | 4)
#define S_msgQLib_ILLEGAL_PRIORITY (M_msgQLib | 5)
#define S_msgQLib_UNSUPPORTED_OPERATION (M_msgQLib | 6)
#define S_msgQLib_INVALID_MSG_COUNT (M_msgQLib | 7)
#define S_msgQLib_ILLEGAL_BUFFER (M_msgQLib | 8)

MSG_Q_ID msgQOpen(const char *name, size_t maxMsgs, size_t maxMsgLength, int options, int mode, void *context);
MSG_Q_ID msgQCreate(size_t maxMsgs, size_t maxMsgLength, int options);

STATUS msgQClose(MSG_Q_ID msgQId);

STATUS msgQSend(MSG_Q_ID msgQId, char *buffer, size_t nBytes, _Vx_ticks_t timeout, int priority);
ssize_t msgQReceive(MSG_Q_ID msgQId, char *buffer, size_t maxNBytes, _Vx_ticks_t timeout);

int msgQNumMsgs(MSG_Q_ID msgQId);

#endif
