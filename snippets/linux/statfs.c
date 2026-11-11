#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/vfs.h>

void
show(const char *name)
{
	struct statfs fs;
	long total, freed;

	if (statfs(name, &fs) < 0) {
		fprintf(stderr, "%s: %s\n", name, strerror(errno));
		return;
	}

	total = fs.f_bsize * fs.f_blocks;
	freed = fs.f_bsize * fs.f_bfree;
	if (total < 0)
		total = 0;
	if (freed < 0)
		freed = 0;

	printf("%s\n", name);
	printf("type:              %lx\n", fs.f_type);
	printf("block size:        %ld\n", fs.f_bsize);
	printf("total blocks:      %ld\n", (long)fs.f_blocks);
	printf("free blocks:       %ld\n", (long)fs.f_bfree);
	printf("available blocks:  %ld\n", (long)fs.f_bavail);
	printf("total file nodes:  %ld\n", (long)fs.f_files);
	printf("free file nodes:   %ld\n", (long)fs.f_ffree);
	printf("max name length:   %ld\n", (long)fs.f_namelen);
	printf("fragment size:     %ld\n", (long)fs.f_frsize);

	printf("\n");
	printf("total size in bytes:     %ld\n", total);
	printf("total size in megabytes: %f\n", total / (1024.0 * 1024.0));

	printf("\n");
	printf("free size in bytes:         %ld\n", freed);
	printf("free size in megabytes:     %f\n", freed / (1024.0 * 1024.0));
	printf("\n");
}

void
usage(void)
{
	fprintf(stderr, "usage: path ...\n");
	exit(2);
}

int
main(int argc, char *argv[])
{
	int i;

	if (argc < 2)
		usage();

	for (i = 1; i < argc; i++)
		show(argv[i]);

	return 0;
}
