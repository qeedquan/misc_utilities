#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

extern char **environ;

void
testfork(void)
{
	static const struct {
		const char *key;
		const char *val;
	} tab[] = {
	    {"ghetto", "blaster"},
	    {"hydra", "pone"},
	    {"real", "limit"},
	};

	const char *val[2];
	pid_t pid;
	size_t i;

	for (i = 0; i < nelem(tab); i++)
		setenv(tab[i].key, tab[i].val, 1);

	switch ((pid = fork())) {
	case 0:
		printf("child:\n");
		for (i = 0; i < nelem(tab); i++) {
			val[0] = getenv(tab[i].key);
			val[1] = secure_getenv(tab[i].key);
			printf("%s: %s %s\n", tab[i].key, val[0], val[1]);
			unsetenv(tab[i].key);
			val[0] = getenv(tab[i].key);
			val[1] = secure_getenv(tab[i].key);
			printf("%s: %s %s\n", tab[i].key, val[0], val[1]);
		}
		break;
	case -1:
		perror("fork");
		exit(1);
		break;
	default:
		waitpid(pid, NULL, 0);
		printf("parent\n");
		for (i = 0; i < nelem(tab); i++) {
			val[0] = getenv(tab[i].key);
			val[1] = secure_getenv(tab[i].key);
			printf("%s: %s %s\n", tab[i].key, val[0], val[1]);
		}
		break;
	}
}

void
testenviron(void)
{
	size_t i;
	printf("\nIn the environment %d\n", getpid());
	for (i = 0; environ[i]; i++)
		printf("%s\n", environ[i]);
}

int
main(void)
{
	testfork();
	testenviron();
	return 0;
}
