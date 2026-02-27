#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <err.h>

typedef struct {
	char shmfile[32];
	char shrfile[32];
	size_t size;
	int shm;
	int use;
} Option;

Option opt;

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define roundup(x, m) ((((x) + (m)-1) / (m)) * m)

void
usage(void)
{
	fprintf(stderr, "usage: [options]\n");
	fprintf(stderr, "\t-f\tspecify the shared memory file (default: %s)\n", opt.shmfile);
	fprintf(stderr, "\t-h\tshow this message\n");
	fprintf(stderr, "\t-n\tspecify the size (default: %zu)\n", opt.size);
	fprintf(stderr, "\t-m\tspecify the shared file\n");
	fprintf(stderr, "\t-s\tuse shm (default: %d)\n", opt.shm);
	fprintf(stderr, "\t-u\tuser (default: %d)\n", opt.use);
	exit(2);
}

void
parseopt(Option *o, int *argc, char ***argv)
{
	int c;

	strcpy(o->shmfile, "/mmap_test");
	o->size = 256 * 1024 * 1024;
	o->shm = 1;
	o->use = 0;
	while ((c = getopt(*argc, *argv, "f:hn:m:s:u")) != -1) {
		switch (c) {
		case 'f':
			snprintf(o->shmfile, sizeof(o->shmfile), "%s", optarg);
			break;
		case 'h':
			usage();
			break;
		case 'n':
			o->size = strtoull(optarg, NULL, 0);
			break;
		case 'm':
			snprintf(o->shrfile, sizeof(o->shrfile), "%s", optarg);
			break;
		case 's':
			o->shm = atoi(optarg);
			break;
		case 'u':
			o->use = 1;
			break;
		}
	}

	o->size = roundup(o->size, getpagesize());

	*argc -= optind;
	*argv += optind;
}

int
copyfile(uint8_t *mem, size_t size, const char *name)
{
	struct stat st;
	FILE *fp;

	if (stat(name, &st) < 0)
		return -errno;

	fp = fopen(name, "rb");
	if (!fp)
		return -errno;

	if ((size_t)st.st_size < size)
		size = st.st_size;

	fread(mem, size, 1, fp);
	fclose(fp);
	return 0;
}

void
share(Option *o)
{
	uint8_t *mem;
	int fd;
	size_t i;
	int r;

	printf("Sharing memory\n");
	printf("Page Size: %d\n", getpagesize());
	printf("File: %s\n", o->shmfile);
	printf("Size: %zu\n", o->size);

	if (o->shm)
		fd = shm_open(o->shmfile, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	else
		fd = open(o->shmfile, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

	if (fd < 0)
		errx(1, "Failed to open file %s: %s", o->shmfile, strerror(errno));

	if (ftruncate(fd, o->size) < 0)
		errx(1, "Failed to truncate file %s: %s", o->shmfile, strerror(errno));

	mem = mmap(NULL, o->size, PROT_WRITE, MAP_SHARED, fd, 0);
	if (mem == MAP_FAILED)
		errx(1, "Failed to map memory: %s\n", strerror(errno));

	if (o->shrfile[0]) {
		r = copyfile(mem, o->size, o->shrfile);
		if (r < 0)
			printf("Failed to serve file: %s\n", strerror(errno));
	} else {
		for (i = 0; i < o->size; i++)
			mem[i] = i & 0xff;
	}

	close(fd);

	printf("Memory: %p\n", mem);
	for (;;) {
		sleep(1);
	}
}

void
use(Option *o)
{
	struct stat st;
	uint8_t *mem;
	size_t i, n;
	int fd;

	printf("Using memory\n");
	printf("File: %s\n", o->shmfile);

	if (o->shm)
		fd = shm_open(o->shmfile, O_RDONLY, S_IRUSR);
	else
		fd = open(o->shmfile, O_RDONLY, S_IRUSR);
	if (fd < 0)
		errx(1, "Failed to open file: %s", strerror(errno));

	if (fstat(fd, &st) < 0)
		errx(1, "Failed to stat file: %s", strerror(errno));

	printf("Size: %zu\n", st.st_size);

	mem = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
	if (mem == MAP_FAILED)
		errx(1, "Failed to mmap file: %s", strerror(errno));

	n = min(st.st_size, 512);
	for (i = 0; i < n; i++) {
		printf("%x ", mem[i]);
		if ((i & 15) == 15)
			printf("\n");
	}
	printf("\n");

	close(fd);
	munmap(mem, st.st_size);
}

int
main(int argc, char *argv[])
{
	parseopt(&opt, &argc, &argv);
	if (!opt.use)
		share(&opt);
	else
		use(&opt);

	return 0;
}
