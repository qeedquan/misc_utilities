#ifndef _FTPLIB_H_
#define _FTPLIB_H_

#include "types/vxTypes.h"
#include "inetLib.h"
#include "remLib.h"
#include "hostLib.h"
#include "taskLibCommon.h"

enum {
	FTP_PRELIM = 0x1,
	FTP_COMPLETE,
	FTP_CONTINUE,
	FTP_TRANSIENT,
	FTP_ERROR,
};

enum {
	FTP_SERVICE_CLOSING = 221,
	FTP_PASSIVE_REPLY = 227,
	FTP_CURRENT_DIRECTORY = 257,
	FTP_CWD_DIRECTORY_OK = 250,
	FTP_COMMAND_NOT_SUPPORTED = 502,
	FTP_NOACTION = 550,
};

STATUS ftpLogin(
    int ctrl,
    char *user,
    char *passwd,
    char *account);

int ftpHookup(char *host);

int ftpDataConnInitPassiveMode(int ctrl);

int ftpCommand(
    int ctrl,
    char *fmt,
    _Vx_usr_arg_t arg1,
    _Vx_usr_arg_t arg2,
    _Vx_usr_arg_t arg3,
    _Vx_usr_arg_t arg4,
    _Vx_usr_arg_t arg5,
    _Vx_usr_arg_t arg6);

int ftpCommandEnhanced(
    int ctrl,
    char *fmt,
    _Vx_usr_arg_t arg1,
    _Vx_usr_arg_t arg2,
    _Vx_usr_arg_t arg3,
    _Vx_usr_arg_t arg4,
    _Vx_usr_arg_t arg5,
    _Vx_usr_arg_t arg6,
    char *replyString,
    int replyStringLength);

STATUS ftpXfer(
    char *host,
    char *user,
    char *passwd,
    char *acct,
    char *cmd,
    char *dirname,
    char *filename,
    int *pCtrlSock,
    int *pDataSock);

int ftpReplyGet(
    int ctrl,
    BOOL expecteof);

int ftpReplyGetEnhanced(
    int ctrl,
    BOOL expecteof,
    char *replyString,
    int stringLengthMax);

int ftpDataConnInit(int ctrl);
int ftpDataConnGet(int data);

#endif
