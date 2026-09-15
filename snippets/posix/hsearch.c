#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <search.h>
#include <err.h>

static char str[32][32] = {
    {"ab"},
    {"bz"},
    {"cA"},
    {"dFD"},
};

int
main(void)
{
	struct hsearch_data *htab;
	char oldstr[32][32];
	size_t i, n;
	ENTRY e, *ep;

	htab = calloc(1, sizeof(*htab));
	hcreate_r(128, htab);

	memcpy(oldstr, str, sizeof(str));
	for (n = 0; n < 10; n++) {
		for (i = 0; str[i][0] != '\0'; i++) {
			// key is not copied but pointed to
			e.key = str[i];
			if (!hsearch_r(e, ENTER, &ep, htab) || ep == NULL)
				err(1, "failed to insert item");

			// insert into the entry returned because
			// if we insert on the first go, we do not get replacement
			// behavior, since hsearch returns the existing entry if the key
			// already exists and does not update the data for us
			printf("inserting %s (%p)\n", str[i], str[i]);
			ep->data = (void *)(uintptr_t)(i + n * 10);
		}

		for (i = 0; oldstr[i][0] != '\0'; i++) {
			e.key = oldstr[i];

			if (!hsearch_r(e, FIND, &ep, htab) || ep == NULL)
				err(1, "failed to find item");

			printf("found %s (%p) %ju\n", ep->key, str[i], (uintmax_t)ep->data);
		}
		printf("--\n");
	}

	hdestroy_r(htab);
	free(htab);

	return 0;
}
