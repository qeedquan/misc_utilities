#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <sys/types.h>
#include <pwd.h>

void
printpw(struct passwd *pw)
{
	int uid, gid;

	uid = pw->pw_uid;
	gid = pw->pw_gid;
	printf("Name:     %s\n", pw->pw_name);
	printf("Password: %s\n", pw->pw_passwd);
	printf("UID:      %d\n", uid);
	printf("GID:      %d\n", gid);
	printf("Gecos:    %s\n", pw->pw_gecos);
	printf("Dir:      %s\n", pw->pw_dir);
	printf("Shell     %s\n", pw->pw_shell);
	printf("\n");
}

void
usage()
{
	errx(2, "usage: user ...");
}

int
main(int argc, char *argv[])
{
	struct passwd *pw;
	int i;

	if (argc < 2)
		usage();

	for (i = 1; i < argc; i++) {
		pw = getpwnam(argv[i]);
		if (!pw && isdigit(argv[i][0]))
			pw = getpwuid(atoi(argv[i]));

		if (!pw) {
			printf("%s: failed lookup\n", argv[i]);
			continue;
		}

		printpw(pw);
	}

	return 0;
}
