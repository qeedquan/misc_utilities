/*

Auxillary vectors are extra information (kind of like environment variables) we can pass to the RTP 
The system uses rtpAuxvAdd() to add predefined RTP variables on startup and those get copied to the RTP while loading it
There are predefined constants provided by the system, but we can add our own constants to store extra info

*/

#include <vxWorks.h>
#include <stdio.h>
#include <sys/auxv.h>

int
main(void)
{
	static const struct Var {
		const char *name;
		int type;
	} vars[] = {
	    {"AT_WINDMEMALN", AT_WINDMEMALN},
	    {"AT_WINDHEAPSZ", AT_WINDHEAPSZ},
	    {"AT_PAGESZ", AT_PAGESZ},
	    {"AT_WINDSTKALN", AT_WINDSTKALN},
	    {"AT_WINDCACHEALN", AT_WINDCACHEALN},
	    {"AT_WINDHEAPOPT", AT_WINDHEAPOPT},
	};

	const struct Var *var;
	auxv_t auxv;
	size_t i;

	for (i = 0; i < NELEMENTS(vars); i++) {
		var = &vars[i];
		if (getauxv(var->type, &auxv) != OK)
			continue;

		printf("%s: %ld\n", var->name, auxv.a_un.a_val);
	}

	return 0;
}
