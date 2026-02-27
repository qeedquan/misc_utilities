#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>
#include <errno.h>

static void
usage(void)
{
    fprintf(stderr, "usage: logname\n");
    exit(1);
}

int
main(int argc, char *argv[])
{
    char *p;
    int c;

    setlocale(LC_ALL, "");
    while ((c = getopt(argc, argv, "h?") != -1)) {
        switch (c) {
        case 'h':
        case '?':
        default:
            usage();
        }
    }

    p = getlogin();
    if (!p) {
        perror("logname");
        exit(1);
    }
    printf("%s\n", p);

    return 0;
}
