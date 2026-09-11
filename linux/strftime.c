#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
    char str[256], *fmt;
    struct tm *tm;
    time_t t;
    int opt, utc;

    utc = 0;
    while ((opt = getopt(argc, argv, "u")) != -1) {
        switch (opt) {
        case 'u':
            utc = 1;
            break;
        }
    }
    argc -= optind;
    argv += optind;

    fmt = "%Y-%m-%d_%02k-%02M";
    if (argc >= 2)
        fmt = argv[1];

    t = time(NULL);
    if (utc)
        tm = gmtime(&t);
    else
        tm = localtime(&t);
   
    if (!tm) {
        perror(utc ? "gmtime" : "localtime");
        return 1;
    }

    if (strftime(str, sizeof(str), fmt, tm) == 0) {
        fprintf(stderr, "invalid strftime format\n");
        return 1;
    }

    printf("%s\n", str);

    return 0;
}
