#include <vxWorks.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <randomNumGen.h>
#include "util.h"

typedef STATUS (*rngfunc)(unsigned char *, int);

void
gen(rngfunc rng)
{
	unsigned char buf[256];
	int r;

	memset(buf, 0, sizeof(buf));
	r = rng(buf, sizeof(buf));

	printf("Status: %d\n", r);
	hexdump(buf, NELEMENTS(buf));
}

int
main(int argc, char *argv[])
{
	static const rngfunc funcs[] = {
	    randBytes,
	    randABytes,
	    randUBytes,
	};

	int i;

	initutil();

	i = 0;
	if (argc >= 2)
		i = atoi(argv[1]);
	assert((size_t)i < NELEMENTS(funcs));

	printf("Random Status: %#lx\n", (long)randStatus());
	printf("Random Secure: %d\n", randSecure());

	gen(funcs[i]);
	return 0;
}
