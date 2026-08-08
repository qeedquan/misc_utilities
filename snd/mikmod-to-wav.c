// cc -o mikmod2wav mikmod-to-wav.c -lmikmod -ldl -lpthread
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>
#include <mikmod.h>
#include <unistd.h>

static void
fatal(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fprintf(stderr, "\n");
	exit(1);
}

static char *
xstrdup(const char *s)
{
	char *p;

	p = strdup(s);
	if (!p)
		fatal("strdup: %s", strerror(errno));
	return p;
}

static char *
xasprintf(char *fmt, ...)
{
	va_list ap;
	char *str;
	int n;

	va_start(ap, fmt);
	n = vasprintf(&str, fmt, ap);
	va_end(ap);

	if (n < 0)
		fatal("asprintf: %s", strerror(errno));

	return str;
}

static void
usage(void)
{
	fatal("usage: in.(s3m|it|xm|mod) [output.wav]");
}

int
main(int argc, char *argv[])
{
	char *cmdline, *filename, *p;
	MODULE *module;

	if (argc < 2)
		usage();

	if (argc > 2)
		filename = argv[2];
	else {
		filename = xstrdup(argv[1]);
		if ((p = strrchr(filename, '.')) && strcmp(p, ".wav"))
			*p = '\0';

		filename = xasprintf("%s.wav", filename);
	}

	cmdline = xasprintf("file=%s", filename);

	MikMod_RegisterDriver(&drv_wav);
	MikMod_RegisterAllLoaders();

	md_device = 1;
	md_mode |= DMODE_SOFT_MUSIC;
	if (MikMod_Init(cmdline))
		fatal("failed to init sound: %s", MikMod_strerror(MikMod_errno));

	module = Player_Load(argv[1], 64, 0);
	if (!module)
		fatal("failed to load file: %s", MikMod_strerror(MikMod_errno));

	printf("converting %s to %s\n", argv[1], filename);

	Player_Start(module);
	while (Player_Active()) {
		usleep(10000);
		MikMod_Update();
	}
	Player_Stop();
	Player_Free(module);
	MikMod_Exit();
	return 0;
}
