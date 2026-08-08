#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <vxWorks.h>
#include <iosLib.h>
#include <fioLib.h>
#include <bootLib.h>
#include <bootApp.h>

static char *
getarg(char *cmd)
{
	unsigned i;

	i = 0;
	while (cmd[i] && cmd[i] != ' ')
		i += 1;
	while (cmd[i] && cmd[i] == ' ')
		i += 1;
	return cmd + i;
}

static int
listdir(char *cmd)
{
	struct dirent *di;
	DIR *dir;
	char *cwd, buf[32];

	cmd = getarg(cmd);
	cwd = getcwd(buf, sizeof(buf));
	if (!cwd)
		cwd = "(nil)";
	bootAppPrintf("current directory: %s\n", (_Vx_usr_arg_t)cwd, 0, 0, 0, 0, 0);

	dir = opendir(cmd);
	if (!dir) {
		bootAppPrintf("failed to open directory: %s: %s\n", (_Vx_usr_arg_t)cmd, (_Vx_usr_arg_t)strerror(errno), 0, 0, 0, 0);
		goto error;
	}

	bootAppPrintf("file listing:\n", 0, 0, 0, 0, 0, 0);
	while ((di = readdir(dir))) {
		bootAppPrintf("%s/%s\n", (_Vx_usr_arg_t)cmd, (_Vx_usr_arg_t)di->d_name, 0, 0, 0, 0);
	}

	if (0) {
	error:;
	}

	if (dir)
		closedir(dir);

	return BOOT_STATUS_COMPLETE;
}

static int
showfile(char *cmd)
{
	FILE *fp;
	int ch;

	cmd = getarg(cmd);
	fp = fopen(cmd, "rb");
	if (!fp) {
		bootAppPrintf("failed to open file: %s: %s\n", (_Vx_usr_arg_t)cmd, (_Vx_usr_arg_t)strerror(errno), 0, 0, 0, 0);
		goto error;
	}

	while ((ch = fgetc(fp)) != EOF)
		bootAppPrintf("%c", ch, 0, 0, 0, 0, 0);

	if (0) {
	error:;
	}

	if (fp)
		fclose(fp);

	return BOOT_STATUS_COMPLETE;
}

static int
testio(char *cmd)
{
	char val, xval;
	int i, times;
	int fd;

	cmd = getarg(cmd);
	bootAppPrintf("testing file io: %s\n", (_Vx_usr_arg_t)cmd, 0, 0, 0, 0, 0);

	fd = open(cmd, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if (fd < 0) {
		bootAppPrintf("failed to open file: %s: %s\n", (_Vx_usr_arg_t)cmd, (_Vx_usr_arg_t)strerror(errno), 0, 0, 0, 0);
		goto error;
	}

	times = 1024;
	val = 0;
	for (i = 0; i < times; i++) {
		if (write(fd, &val, sizeof(val)) != sizeof(val)) {
			bootAppPrintf("failed to write value: %d %d: %s\n", i, val, (_Vx_usr_arg_t)strerror(errno), 0, 0, 0);
			goto error;
		}

		val = (val + 1) & 0x7f;
	}

	lseek(fd, 0, SEEK_SET);

	val = 0;
	for (i = 0; i < times; i++) {
		if (read(fd, &xval, sizeof(xval)) != sizeof(xval)) {
			bootAppPrintf("failed to read value: %d %d: %s\n", i, val, (_Vx_usr_arg_t)strerror(errno), 0, 0, 0);
			goto error;
		}
		if (xval != val) {
			bootAppPrintf("mismatch %d | %d %d\n", i, val, xval, 0, 0, 0);
		}
		val = (val + 1) & 0x7f;
	}

	if (0) {
	error:;
	}

	if (fd >= 0)
		close(fd);

	return BOOT_STATUS_COMPLETE;
}

static int
iosshow(char *cmd)
{
	iosDevShow();
	iosDrvShow();
	iosFdShow();

	return BOOT_STATUS_COMPLETE;
}

static int
fileio(char *cmd)
{
	FILE *fp;
	char line[160];
	size_t len;

	cmd = getarg(cmd);
	fp = fopen(cmd, "wb");
	if (!fp) {
		bootAppPrintf("failed to open file: %s: %s\n", (_Vx_usr_arg_t)cmd, (_Vx_usr_arg_t)strerror(errno), 0, 0, 0, 0);
		goto error;
	}

	for (;;) {
		bootAppPrintf("> ", 0, 0, 0, 0, 0, 0);
		if (fioRdString(STD_IN, line, sizeof(line) - 2) == EOF)
			break;

		len = strlen(line);
		line[len++] = '\n';
		line[len] = EOS;
		if (fwrite(line, len, 1, fp) != 1) {
			bootAppPrintf("error writing to file: %s\n", (_Vx_usr_arg_t)strerror(errno), 0, 0, 0, 0, 0);
			break;
		}
	}

	if (0) {
	error:;
	}

	if (fp)
		fclose(fp);

	return BOOT_STATUS_COMPLETE;
}

void
initbootdbg(void)
{
	bootCommandHandlerAdd("ls", listdir, BOOT_CMD_MATCH_STRICT, "ls dir", "- list directory");
	bootCommandHandlerAdd("cat", showfile, BOOT_CMD_MATCH_STRICT, "cat file", "- output file contents");
	bootCommandHandlerAdd("ios", iosshow, BOOT_CMD_MATCH_STRICT, "ios", "- list all IOS drivers");
	bootCommandHandlerAdd("fio", fileio, BOOT_CMD_MATCH_STRICT, "fio file", "- read input and write it to file");
	bootCommandHandlerAdd("tio", testio, BOOT_CMD_MATCH_STRICT, "tio file", "- test io on file");
}
