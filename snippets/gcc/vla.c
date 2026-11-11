// https://lemon.rip/w/c99-vla-tricks/

#include <stdio.h>

void
f(char[(
    printf("Press enter to confirm: "),
    getchar(),
    printf("thanks.\n"),
    1)])
{
}

int
main(void)
{
	f(NULL);

	return 0;
}
