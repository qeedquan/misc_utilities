#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <windows.h>
#include <dbghelp.h>

#define MAXLINE 32768

void WINAPI
demangle(FILE *fp)
{
	char buf[MAXLINE];
	char name[MAXLINE];
	char sym[MAXLINE];
	size_t i, j, len;

	while (fgets(buf, sizeof(buf), fp)) {
		len = strlen(buf);
		if (len > 0 && buf[len - 1] == '\n')
			buf[len - 1] = '\0';

		for (i = 0; i < len; i++) {
			if (buf[i] != '?') {
				printf("%c", buf[i]);
				continue;
			}

			j = i + 1;
			while (j < len && !isspace(buf[j]))
				j++;

			memcpy(name, buf + i, j - i);
			name[j - i - 1] = '\0';
			UnDecorateSymbolName(name, sym, sizeof(sym), UNDNAME_COMPLETE);
			printf("%s", sym);
			
			i = j - 1;
		}
		printf("\n");
	}
}

int WINAPI
main(int argc, char *argv[])
{
	FILE *fp;
	int i;

	if (argc < 2)
		demangle(stdin);
	else {
		for (i = 1; i < argc; i++) {
			fp = fopen(argv[i], "rb");
			if (!fp) {
				printf("%s: %s\n", argv[i], strerror(errno));
				continue;
			}
			demangle(fp);
			fclose(fp);
		}
	}

	return 0;
}
