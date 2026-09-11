/*

memdrv is a in memory way to create a filesystem, this is like ramdrv but you get more control in specifying which memory location the files/directories are created at

*/

#include <vxWorks.h>
#include <memDrv.h>

/*

Create a /memfs folder with contents

alpha/
bravo
charlie/
        delta
        echo/
                foxtrot
                golf

*/

void
test_memdrv_directory(void)
{
	enum fsoffset {
		ALPHA = 0,
		BRAVO = 6,
		CHARLIE = 12,
		DELTA = 20,
		ECHO = 26,
		FOXTROT = 31,
		GOLF = 39,
	};

	static char fsdata[] = "ALPHA\nBRAVO\nCHARLIE\nDELTA\nECHO\nFOXTROT\nGOLF\n";
	static MEM_DRV_DIRENTRY entries[] = {
	    {"alpha", fsdata + ALPHA, entries, 0},
	    {"bravo", fsdata + BRAVO, NULL, sizeof("BRAVO")},
	    {"charlie", fsdata + CHARLIE, &entries[3], 2},
	    {"delta", fsdata + DELTA, NULL, sizeof("DELTA")},
	    {"echo", fsdata + ECHO, &entries[5], 2},
	    {"foxtrot", fsdata + FOXTROT, NULL, sizeof("FOXTROT")},
	    {"golf", fsdata + GOLF, NULL, sizeof("GOLF")},
	};

	memDevCreateDir("/memfs", entries, 3);
}
