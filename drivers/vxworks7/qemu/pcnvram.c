#include <vxWorks.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <sysLib.h>
#include <ioLib.h>
#include <iosLib.h>
#include <semLib.h>
#include <errnoLib.h>

enum {
	Paddr = 0x70,
	Pdata = 0x71,

	Nvoff = 128,
	Nvsize = 256,
};

typedef struct {
	size_t pos;
} NV;

static DEV_HDR nvdevhdr;
static SEM_ID nvsem;

LOCAL void *
nvopen(DEV_HDR *devhdr, const char *name, int flags, int mode)
{
	NV *nv;

	nv = calloc(1, sizeof(*nv));
	if (!nv)
		return (void *)ERROR;

	return nv;

	(void)devhdr;
	(void)name;
	(void)flags;
	(void)mode;
}

LOCAL ssize_t
nvio(int op, void *ctx, void *buf, size_t len)
{
	NV *nv;
	char *p;
	size_t off, n;

	semTake(nvsem, WAIT_FOREVER);
	nv = ctx;
	p = buf;
	off = nv->pos;
	for (n = 0; n < len; n++) {
		if (off + n >= Nvsize)
			break;

		sysOutByte(Paddr, Nvoff + off + n);
		if (op == 'r')
			p[n] = sysInByte(Pdata);
		else if (op == 'w')
			sysOutByte(Pdata, p[n]);
	}
	nv->pos = off + n;
	semGive(nvsem);
	return n;
}

LOCAL ssize_t
nvread(void *ctx, char *buf, size_t len)
{
	return nvio('r', ctx, buf, len);
}

LOCAL ssize_t
nvwrite(void *ctx, const char *buf, size_t len)
{
	return nvio('w', ctx, (void *)buf, len);
}

LOCAL int
nvclose(void *ctx)
{
	free(ctx);
	return OK;
}

LOCAL STATUS
nvioctl(void *ctx, int func, _Vx_ioctl_arg_t arg)
{
	struct stat *st;
	int64_t off;
	NV *nv;
	STATUS r;

	semTake(nvsem, WAIT_FOREVER);
	r = OK;
	nv = ctx;
	switch (func) {
	case FIOWHERE:
		r = nv->pos;
		break;
	case FIOWHERE64:
		*(int64_t *)arg = nv->pos;
		break;

	case FIOSEEK:
	case FIOSEEK64:
		off = arg;
		if (func == FIOSEEK64)
			off = *(int64_t *)arg;
		nv->pos = max(0, min(off, Nvsize));
		r = nv->pos;
		break;

	case FIOFSTATGET:
		st = (struct stat *)arg;
		memset(st, 0, sizeof(*st));
		st->st_size = Nvsize;
		break;

	default:
		errnoSet(ENOTSUP);
		r = ERROR;
		break;
	}
	semGive(nvsem);

	return r;
}

STATUS
nvdevcreate(void)
{
	int iosdrvnum;

	nvsem = semBCreate(SEM_Q_PRIORITY, SEM_FULL);
	iosdrvnum = iosDrvInstall(NULL, NULL, nvopen, nvclose, nvread, nvwrite, nvioctl);
	if (nvsem == SEM_ID_NULL || iosdrvnum == ERROR)
		goto error;

	if (iosDevAdd(&nvdevhdr, "/pcnvram", iosdrvnum) == ERROR)
		goto error;

	return OK;

error:
	if (nvsem)
		semDelete(nvsem);
	return ERROR;
}

void
nvtest(void)
{
	struct stat st;
	unsigned char buf[Nvsize];
	ssize_t nr;
	int64_t pos;
	int fd, i, r;

	fd = open("/pcnvram", O_RDWR);
	if (fd < 0)
		return;

	fstat(fd, &st);
	printf("nvramsize: %zu\n", st.st_size);

	for (i = 0; i < 8; i++) {
		nr = read(fd, buf, 0x80);
		printf("read: %zd\n", nr);

		printf("where: %d\n", ioctl(fd, FIOWHERE, NULL));
		r = ioctl(fd, FIOWHERE64, &pos);
		printf("where64: %d %" PRId64 "\n", r, pos);

		if (i == 4)
			printf("seek: %ld\n", (long)lseek(fd, 3, SEEK_SET));
	}

	printf("seek: %ld\n", (long)lseek(fd, 0x20, SEEK_SET));
	for (i = 0; i < 16; i++)
		buf[i] = 0x55;
	printf("write: %zd\n", write(fd, buf, 16));

	printf("seek: %ld\n", (long)lseek(fd, 0, SEEK_SET));
	printf("read: %zd\n", read(fd, buf, Nvsize));
	printf("\n");
	for (i = 0; i < Nvsize; i++) {
		printf("%02x ", buf[i]);
		if ((i & 15) == 15)
			printf("\n");
	}

	close(fd);
}
