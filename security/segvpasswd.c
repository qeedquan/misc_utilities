#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <err.h>

#define MAXPASS 128

void
genpass(char *b, size_t n)
{
	size_t i;

	if (n == 0)
		return;

	for (i = 0; i < n - 1; i++)
		b[i] = 32 + (rand() % 95);
	b[i] = '\0';
}

int
checkpass(const char *s, const char *t)
{
	size_t i;
	for (i = 0; s[i]; i++) {
		if (s[i] != t[i])
			return 0;
	}
	return 1;
}

jmp_buf env;

void
segvhandle(int sig, siginfo_t *si, void *uc)
{
	siglongjmp(env, 1);

	(void)sig;
	(void)si;
	(void)uc;
}

void
sim(void)
{
	char secret[MAXPASS], pass[MAXPASS], *buf;
	size_t npass, pagesz, i;
	struct sigaction sa;

	sa.sa_flags = SA_SIGINFO;
	sigemptyset(&sa.sa_mask);
	sa.sa_sigaction = segvhandle;
	if (sigaction(SIGSEGV, &sa, NULL) < 0)
		errx(1, "failed to set signal handler: %s", strerror(errno));

	pagesz = getpagesize();
	if (pagesz < MAXPASS)
		errx(1, "page size of %zu bytes is too small for max password of %d bytes", pagesz, MAXPASS);

	genpass(secret, (rand() % sizeof(secret)) + 1);

	buf = aligned_alloc(pagesz, pagesz * 2);
	if (!buf)
		errx(1, "failed to allocate memory for password buffer");
	if (mprotect(buf + pagesz, pagesz, PROT_NONE) != 0)
		errx(1, "failed to set io permission for password buffer: %s", strerror(errno));

	printf("%s\n", secret);

	memset(pass, 0, sizeof(pass));
	npass = 0;

	if (sigsetjmp(env, 1))
		npass++;
	memmove(buf + pagesz - npass - 1, pass, npass);
	for (i = 32; i < 127; i++) {
		buf[pagesz - 1] = pass[npass] = i;
		if (checkpass(secret, buf + pagesz - npass - 1))
			break;
	}
	printf("%s\n", pass);
	if (strcmp(secret, pass))
		printf("failed to match password\n");

	free(buf);
}

int
main(void)
{
	srand(time(NULL));
	sim();
	return 0;
}
