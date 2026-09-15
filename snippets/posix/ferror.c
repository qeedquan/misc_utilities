#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <err.h>
#include <unistd.h>

void
testio(const char *name, const char *mode, int clear)
{
	FILE *fp;
	char buf[8192];
	size_t r;
	int i;

	printf("Testing read/write on %s with mode %s clear %d\n", name, mode, clear);
	fp = fopen(name, mode);
	if (!fp)
		err(1, "open");

	// reading more after EOF does not set the error flag
	// it only sets it if the underlying os returns an error
	for (i = 0; i < 10; i++) {
		r = fread(buf, 1, sizeof(buf), fp);
		printf("ferror %d ret %zu\n", ferror(fp), r);
	}

	// writing to a read only file sets the flag
	for (i = 0; i < 10; i++) {
		r = fwrite(buf, 1, sizeof(buf), fp);
		printf("ferror %d ret %zu\n", ferror(fp), r);
	}

	// there is a standard way to clear the error
	// but no standard way to set it
	// musl comes with __fseterr but glibc doesn't
	if (clear) {
		clearerr(fp);
		printf("ferror %d\n", ferror(fp));
	}

	// even if ferror is set, fclose won't return an error
	// so we need to check fclose return code too
	printf("fclose %d\n", fclose(fp));
	printf("\n");
}

// portable way to do fseterr, from gnulib
void
fseterr(FILE *fp)
{
	int saved_errno;
	int fd;
	int fd2;

	saved_errno = errno;
	fflush(fp);
	fd = fileno(fp);
	fd2 = dup(fd);
	if (fd2 >= 0) {

		close(fd);

		// this sets the error
		fputc('\0', fp);
		fflush(fp);

		// something is seriously wrong if this happens
		if (dup2(fd2, fd) < 0)
			abort();

		close(fd2);
	}
	errno = saved_errno;
}

void
testset(void)
{
	FILE *fp;
	int i;

	printf("Testing set error\n");
	fp = fopen(".", "rb");
	if (!fp)
		err(1, "open");

	for (i = 0; i < 10; i++) {
		fseterr(fp);
		printf("ferror %d\n", ferror(fp));
		clearerr(fp);
		printf("ferror %d\n", ferror(fp));
	}
	fclose(fp);
}

int
main(void)
{
	testio("/etc/passwd", "rb", 1);
	testio("/dev/null", "rb", 1);
	testio("/dev/null", "wb", 0);
	testset();

	return 0;
}
