#include <vxWorks.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysLib.h>
#include <errno.h>
#include <unistd.h>
#include <getopt.h>
#include <syscall.h>

void
usage(void)
{
	fprintf(stderr, "usage: syscall_show [-a(ll) -g(roups) -s <syscallgroup num>]\n");
	exit(2);
}

void
scGroupSummaryShow(void)
{
	char buf[128];
	size_t bufsz;
	int group;
	int numrtn;

	printf("\nGroup Name              GroupNo   NumRtns\n"
	       "----------------------  -------   -------\n");

	for (group = 0; group < (1 << SYSCALL_GROUP_NUM_BITS); group++) {
		bufsz = sizeof(buf);
		if (syscallGroupNameGet(group, buf, &bufsz) == -1)
			continue;

		if (syscallGroupNumRtnGet(group, &numrtn) == -1)
			continue;

		printf("%-22s   %-6d    %-5d\n", buf, group, numrtn);
	}

	printf("\n");
}

void
scGroupDetailShow(int group, BOOL verbose)
{
	char buf[128];
	size_t bufsz;
	BOOL scNotRegistered;
	BOOL scUnknownArgNum;
	int syscallNum;
	int section;
	int numargs;
	int numrtn;

	bufsz = sizeof(buf);
	if (syscallGroupNameGet(group, buf, &bufsz) == -1) {
		if (verbose)
			printf("System group number %d does not exist.\n", group);
		return;
	}

	if (syscallGroupNumRtnGet(group, &numrtn) == -1) {
		if (verbose)
			printf("System group number %d does not have system calls.\n", group);
		return;
	}

	printf("\nSystem Call Group name: %s\n"
	       "Group Number          : %d\n\n",
	       buf, group);

	printf("Routines provided     : %d (reported as \"(null)\" when not "
	       "installed)\n"
	       "Rtn#   Name                    # Arguments\n"
	       "----   ----------------------  -----------\n",
	       numrtn);

	syscallNum = group << SYSCALL_GROUP_NO_BIT_START;
	for (section = 0; section < numrtn; section++) {
		bufsz = sizeof(buf);
		scNotRegistered = FALSE;
		scUnknownArgNum = FALSE;

		if (syscallPresent(syscallNum + section, buf, &bufsz) == -1)
			scNotRegistered = TRUE;
		else if (syscallNumArgsGet(syscallNum + section, &numargs) == -1)
			scUnknownArgNum = TRUE;

		if (!scNotRegistered && !scUnknownArgNum) {
			printf("%-6d %-22s     %d\n", section, buf, numargs);
		} else {
			if (scNotRegistered) {
				printf("%-6d (null)                     0\n", section);
			} else {
				printf("%-6d %-22s     ?\n", section, buf);
			}
		}
	}
}

int
main(int argc, char *argv[])
{
	int group;
	int ch;
	BOOL showall;
	BOOL showgroups;

	showall = FALSE;
	showgroups = FALSE;

	if (argc == 1)
		showgroups = TRUE;

	while ((ch = getopt(argc, argv, "ags:")) != EOF) {
		switch (ch) {
		case 's':
			group = atoi(optarg);
			scGroupDetailShow(group, TRUE);
			break;
		case '?':
			usage();
			break;
		case 'a':
			showall = TRUE;
			break;
		case 'g':
		default:
			showgroups = TRUE;
			break;
		}
	}

	if (showgroups)
		scGroupSummaryShow();

	if (showall) {
		for (group = 0; group < (1 << SYSCALL_GROUP_NUM_BITS); group++) {
			scGroupDetailShow(group, FALSE);
		}
	}

	return 0;
}
