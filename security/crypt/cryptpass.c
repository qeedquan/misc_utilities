// cc -o cryptpass cryptpass.c -lcrypt
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <getopt.h>
#include <err.h>
#include <crypt.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

typedef struct {
	const char *prefix;
	const char *key;
	char hashpw[512];
	char salt[128];
	size_t saltsz;
	int nosalt;
} Option;

void
usage(void)
{
	fprintf(stderr, "usage: cryptpass [options] password [hash_password]\n");
	fprintf(stderr, "  -a, --algorithm     use encryption algorithm (default: sha512)\n");
	fprintf(stderr, "  -s, --salt          use salt (default: random)\n");
	fprintf(stderr, "  -v, --verify        verify password against the hash of the password\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "available encryption algorithms: des md5 sha256 sha512\n");
	exit(2);
}

void
gensalt(char *salt, size_t len)
{
	static const char alphabet[] =
	    "abcdefghijklmopqrstuvwxyz"
	    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	    "0123456789_/";

	FILE *fp;
	size_t i;

	fp = fopen("/dev/urandom", "rb");
	if (fp == NULL)
		errx(1, "failed to open random device: %s", strerror(errno));

	if (fread(salt, len, 1, fp) != 1)
		errx(1, "failed to gen random salt: %s", strerror(errno));

	for (i = 0; i < len; i++) {
		salt[i] = alphabet[salt[i] % (nelem(alphabet) - 1)];
		if (len == 2 && salt[i] == '_')
			salt[i] = '.';
	}
	salt[len] = '\0';

	fclose(fp);
}

void
getalg(Option *o, const char *name)
{
	static const struct {
		const char *name;
		const char *prefix;
		size_t saltsz;
	} tab[] = {
	    {"des", "", 2},
	    {"md5", "$1$", 22},
	    {"sha256", "$5$", 43},
	    {"sha512", "$6$", 86},
	};
	size_t i;

	o->prefix = NULL;
	for (i = 0; i < nelem(tab); i++) {
		if (strcasecmp(name, tab[i].name) == 0) {
			o->prefix = tab[i].prefix;
			o->saltsz = tab[i].saltsz;
			return;
		}
	}
}

void
parseopt(Option *o, int *argc, char ***argv)
{
	static const struct option options[] = {
	    {"algorithm", optional_argument, 0, 0},
	    {"salt", optional_argument, 0, 0},
	    {"verify", optional_argument, 0, 0},
	};
	int index, c;
	size_t n;

	getalg(o, "sha512");
	o->nosalt = 0;
	o->salt[0] = '\0';

	while ((c = getopt_long(*argc, *argv, "a:s:v:", options, &index)) != -1) {
		switch (c) {
		case 0:
			switch (index) {
			case 0:
				goto oalg;
			case 1:
				goto osalt;
			case 2:
				goto overify;
			}
			break;
		case 'a':
		oalg:
			getalg(o, optarg);
			if (o->prefix == NULL)
				errx(1, "invalid algorithm '%s'", optarg);
			break;
		case 's':
		osalt:
			snprintf(o->salt, sizeof(o->salt), "%s", optarg);
			if (o->salt[0] == '\0')
				o->nosalt = 1;
			break;
		case 'v':
		overify:
			snprintf(o->hashpw, sizeof(o->hashpw), "%s", optarg);
			break;
		}
	}

	*argc -= optind;
	*argv += optind;
	if (*argc < 1)
		usage();

	n = strlen(o->salt);
	if (n > 0 && n != o->saltsz)
		errx(1, "invalid salt '%s'", o->salt);
	else if (n == 0)
		gensalt(o->salt, o->saltsz);

	o->key = *argv[0];
}

void
verify(Option *o)
{
	char *result;

	result = crypt(o->key, o->hashpw);
	if (!strcmp(result, o->hashpw))
		printf("password match\n");
	else
		printf("password mismatch\n");
}

void
docrypt(Option *o)
{
	char mode[256], *enc;
	int len;

	if (o->nosalt)
		snprintf(mode, sizeof(mode), "%s", o->prefix);
	else
		snprintf(mode, sizeof(mode), "%s%s", o->prefix, o->salt);

	enc = crypt(o->key, mode);
	if (enc == NULL) {
		fprintf(stderr, "mode: %s\n", mode);
		errx(1, "error generating password: %s", strerror(errno));
	}
	printf("%s\n", enc);
}

int
main(int argc, char *argv[])
{
	Option opt;

	parseopt(&opt, &argc, &argv);
	if (opt.hashpw[0])
		verify(&opt);
	else
		docrypt(&opt);

	return 0;
}
