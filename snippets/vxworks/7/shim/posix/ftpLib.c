#include "vxWorks.h"
#include "ftpLib.h"

enum {
	PASV_REPLY_STRING_LENGTH = 256,
	FTP_CMD_BUFFER_LENGTH = 259,
};

typedef struct {
	char username[MAX_IDENTITY_LEN];
	char password[MAX_IDENTITY_LEN];
	char hostname[MAXHOSTNAMELEN];
	int reply_timeout;
	bool passive_mode_disable;
} FTP;

static thread_local FTP ftp = {
    .reply_timeout = 10,
    .passive_mode_disable = false,
};

STATUS
ftpLogin(
    int ctrl,
    char *user,
    char *passwd,
    char *account)
{
	size_t userlen;
	size_t passwdlen;
	int r;

	userlen = strlen(user);
	passwdlen = strlen(passwd);
	if (userlen >= MAX_IDENTITY_LEN || passwdlen >= MAX_IDENTITY_LEN)
		return ERROR;

	r = ftpCommand(ctrl, "USER %s", (_Vx_usr_arg_t)user, 0, 0, 0, 0, 0);
	if (r == FTP_CONTINUE)
		r = ftpCommand(ctrl, "PASS %s", (_Vx_usr_arg_t)passwd, 0, 0, 0, 0, 0);
	if (r != FTP_COMPLETE)
		return ERROR;

	snprintf(ftp.username, sizeof(ftp.username), "%s", user);
	snprintf(ftp.password, sizeof(ftp.password), "%s", passwd);
	return OK;
}

int
ftpHookup(char *host)
{
	struct sockaddr_in addr;
	struct linger lingopt;
	in_addr_t in_addr;
	int retry;
	int intopt;
	int ctrl;

	ctrl = -1;
	in_addr = inet_addr(host);
	if (in_addr == INADDR_NONE)
		goto error;

	ctrl = socket(AF_INET, SOCK_STREAM, 0);
	if (ctrl < 0)
		goto error;

	lingopt.l_onoff = 1;
	lingopt.l_linger = 0;
	if (setsockopt(ctrl, SOL_SOCKET, SO_LINGER, &lingopt, sizeof(lingopt)) < 0)
		goto error;

	intopt = 1;
	if (setsockopt(ctrl, SOL_SOCKET, SO_KEEPALIVE, &intopt, sizeof(intopt)) < 0)
		goto error;

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(0);
	if (bind(ctrl, &addr, sizeof(addr)) < 0)
		goto error;

	addr.sin_addr.s_addr = in_addr;
	addr.sin_port = htons(21);

	retry = 0;
	for (;;) {
		if (connect(ctrl, &addr, sizeof(addr)) >= 0)
			break;

		switch (errno) {
		case EHOSTUNREACH:
		case ENETUNREACH:
		case ENETDOWN:
			if (retry++ < ftp.reply_timeout) {
				sleep(1);
				continue;
			}
			// fallthrough
		default:
			goto error;
		}
	}

	if (ftpReplyGet(ctrl, FALSE) != FTP_COMPLETE)
		goto error;

	snprintf(ftp.hostname, sizeof(ftp.hostname), "%s", host);

	if (0) {
	error:
		if (ctrl >= 0)
			close(ctrl);
		ctrl = -1;
	}

	return ctrl;
}

static STATUS
ftpPasvReplyParse(char *reply, UINT32 *arg1, UINT32 *arg2, UINT32 *arg3, UINT32 *arg4, UINT32 *arg5, UINT32 *arg6)
{
	UINT32 *parg[] = {arg1, arg2, arg3, arg4, arg5, arg6};
	char *index;
	int val[6];
	int i;

	if (reply == NULL)
		return ERROR;

	if (strstr(reply, "227") == NULL)
		return ERROR;

	index = strstr(reply, "(");
	if (index == NULL)
		return ERROR;

	if (sscanf(index + 1, "%d,%d,%d,%d,%d,%d", val, val + 1, val + 2, val + 3, val + 4, val + 5) != 6)
		return ERROR;

	for (i = 0; i < 6; i++) {
		if (parg[i])
			*parg[i] = val[i];
	}

	return OK;
}

int
ftpDataConnInitPassiveMode(int ctrl)
{
	struct sockaddr_in ctrl_addr;
	struct sockaddr_in data_addr;
	socklen_t len;
	char reply[PASV_REPLY_STRING_LENGTH];
	UINT32 hi, lo;
	int port;
	int data;
	int r;

	data = -1;
	if (ftp.passive_mode_disable)
		goto error;

	len = sizeof(ctrl_addr);
	if (getsockname(ctrl, (struct sockaddr *)&ctrl_addr, &len) < 0)
		goto error;

	r = ftpCommandEnhanced(ctrl, "PASV", 0, 0, 0, 0, 0, 0, reply, sizeof(reply) - 1);
	if (r != FTP_PASSIVE_REPLY)
		goto error;

	if (ftpPasvReplyParse(reply, 0, 0, 0, 0, &hi, &lo) == ERROR)
		goto error;

	port = (hi << 8) | lo;
	data = socket(AF_INET, SOCK_STREAM, 0);
	if (data < 0)
		goto error;

	len = sizeof(ctrl_addr);
	if (getpeername(ctrl, (struct sockaddr *)&ctrl_addr, &len) < 0)
		goto error;

	memset(&data_addr, 0, sizeof(data_addr));
	data_addr.sin_port = htons(port);
	data_addr.sin_family = AF_INET;
	data_addr.sin_addr.s_addr = ctrl_addr.sin_addr.s_addr;

	if (connect(data, (struct sockaddr *)&data_addr, sizeof(data_addr) < 0))
		goto error;

	if (0) {
	error:
		if (data >= 0)
			close(data);
		data = -1;
	}

	return data;
}

int
ftpCommand(
    int ctrl,
    char *fmt,
    _Vx_usr_arg_t arg1,
    _Vx_usr_arg_t arg2,
    _Vx_usr_arg_t arg3,
    _Vx_usr_arg_t arg4,
    _Vx_usr_arg_t arg5,
    _Vx_usr_arg_t arg6)
{
	int r;

	r = ftpCommandEnhanced(ctrl, fmt, arg1, arg2, arg3, arg4, arg5, arg6, NULL, 0);
	return (r > 0) ? (r / 100) : ERROR;
}

int
ftpCommandEnhanced(
    int ctrl,
    char *fmt,
    _Vx_usr_arg_t arg1,
    _Vx_usr_arg_t arg2,
    _Vx_usr_arg_t arg3,
    _Vx_usr_arg_t arg4,
    _Vx_usr_arg_t arg5,
    _Vx_usr_arg_t arg6,
    char *replyString,
    int replyStringLength)
{
	char buf[FTP_CMD_BUFFER_LENGTH];
	int len;
	int eof;

	len = snprintf(buf, sizeof(buf) - 3, fmt, arg1, arg2, arg3, arg4, arg5, arg6);
	sprintf(&buf[len], "\r\n");
	if (write(ctrl, buf, len + 2) != len + 2)
		return ERROR;

	eof = strcmp(fmt, "QUIT") == 0;

	return ftpReplyGetEnhanced(ctrl, eof, replyString, replyStringLength);
}

STATUS
ftpXfer(
    char *host,
    char *user,
    char *passwd,
    char *acct,
    char *cmd,
    char *dirname,
    char *filename,
    int *pCtrlSock,
    int *pDataSock)
{
	bool passive;
	int reply;
	int ctrl;
	int data;
	int r;

	r = OK;
	ctrl = ftpHookup(host);
	data = -1;
	if (ctrl < 0)
		goto error;

	if (ftpLogin(ctrl, user, passwd, acct) != OK)
		goto error;

	if (ftpCommand(ctrl, "TYPE I", 0, 0, 0, 0, 0, 0) != FTP_COMPLETE)
		goto error;

	if (dirname[0] && ftpCommand(ctrl, "CWD %s", (_Vx_usr_arg_t)dirname, 0, 0, 0, 0, 0) != FTP_COMPLETE)
		goto error;

	if (pDataSock == NULL) {
		if (ftpCommand(ctrl, cmd, (_Vx_usr_arg_t)filename, 0, 0, 0, 0, 0) == ERROR)
			goto error2;
	}

	passive = false;
	reply = 0;
	do {
		if ((data = ftpDataConnInitPassiveMode(ctrl)) != ERROR) {
			passive = true;
		} else {
			if ((data = ftpDataConnInit(ctrl)) == ERROR)
				goto error;
			passive = false;
		}
	} while (reply == FTP_TRANSIENT);

	if (passive) {
	}

	if (0) {
	error:
		if (ctrl >= 0)
			close(ctrl);
		ctrl = -1;

	error2:
		if (data >= 0)
			close(data);
		data = -1;

		r = ERROR;
	}

	if (pCtrlSock)
		*pCtrlSock = ctrl;

	if (pDataSock)
		*pDataSock = data;

	return r;
}

int
ftpReplyGet(int ctrl, BOOL expecteof)
{
	int r;

	r = ftpReplyGetEnhanced(ctrl, expecteof, NULL, 0);
	return (r > 0) ? r / 100 : ERROR;
}

int
ftpReplyGetEnhanced(
    int ctrl,
    BOOL expecteof,
    char *replyString,
    int stringLengthMax)
{
	struct pollfd pfd[1];
	size_t si;
	int di;
	int eof;
	int cont;
	int orig;
	int code;
	char c;

	pfd[0].fd = ctrl;
	pfd[0].events = POLLIN;
	pfd[0].revents = 0;

	orig = 0;

	do {
		if (poll(pfd, nelem(pfd), ftp.reply_timeout * 1000) < 0)
			return ERROR;

		di = 0;
		si = 0;
		for (;;) {
			eof = read(ctrl, &c, 1);
			if (eof < 0) {
				if (errno == EINTR)
					continue;
			}

			if (eof <= 0 || c == '\n')
				break;

			if (replyString && si < stringLengthMax)
				replyString[si++] = c;

			di++;
			if (di <= 3) {
				if (!isdigit(c))
					code = -1;
				else if (code != -1)
					code = (code * 10) + (c - '0');
			}
			if (di == 4)
				cont = (c == '-');
		}

		if (replyString && si < stringLengthMax)
			replyString[si++] = '\0';

		if (orig == 0)
			orig = code;
	} while (eof > 0 && !(di >= 3 && code == orig && !cont));

	if (expecteof && eof < 0 && errno == ECONNRESET) {
		eof = 0;
		orig = FTP_SERVICE_CLOSING;
	}

	if (eof < 0)
		return ERROR;

	if (eof == 0 && !expecteof)
		return ERROR;

	if (orig == 0)
		return ERROR;

	return orig;
}

int
ftpDataConnInit(int ctrl)
{
	struct sockaddr_in ctrl_addr;
	struct sockaddr_in data_addr;
	UINT32 ip, port;
	socklen_t len;
	int data;
	int opt;
	int r;

	data = -1;
	len = sizeof(ctrl_addr);
	if (getsockname(ctrl, (struct sockaddr *)&ctrl_addr, &len) < 0)
		goto error;

	data = socket(AF_INET, SOCK_STREAM, 0);
	if (data < 0)
		goto error;

	data_addr = ctrl_addr;
	data_addr.sin_port = htons(0);
	if (bind(data, (struct sockaddr *)&data_addr, sizeof(data_addr)) < 0)
		goto error;

	if (listen(data, 1) < 0)
		goto error;

	len = sizeof(data_addr);
	if (getsockname(data, (struct sockaddr *)&data_addr, &len) < 0)
		goto error;

	ip = data_addr.sin_addr.s_addr;
	port = data_addr.sin_port;
	r = ftpCommand(ctrl, "PORT %d,%d,%d,%d,%d,%d",
	               ip & 0xff, (ip >> 8) & 0xff, (ip >> 16) & 0xff, (ip >> 24) & 0xff,
	               port & 0xff, (port >> 8) & 0xff);

	if (r != FTP_ERROR) {
		if (r != FTP_COMPLETE && r != FTP_PRELIM)
			goto error;

		return data;
	}

	close(data);
	data = socket(AF_INET, SOCK_STREAM, 0);
	if (data < 0)
		goto error;

	opt = 1;
	if (setsockopt(data, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		goto error;

	if (bind(data, (struct sockaddr *)&ctrl_addr, (socklen_t)sizeof(ctrl_addr)) < 0)
		goto error;

	if (listen(data, 1) < 0)
		goto error;

	if (0) {
	error:
		if (data >= 0)
			close(data);
		data = -1;
	}

	return data;
}

int
ftpDataConnGet(int data)
{
	struct pollfd pfd[1];
	struct sockaddr_in from;
	socklen_t fromlen;
	int new_data;

	pfd[0].fd = data;
	pfd[0].events = POLLIN;
	pfd[0].revents = 0;
	if (poll(pfd, nelem(pfd), ftp.reply_timeout * 1000) < 0)
		return ERROR;

	new_data = accept(data, (struct sockaddr *)&from, &fromlen);
	close(data);

	return new_data;
}