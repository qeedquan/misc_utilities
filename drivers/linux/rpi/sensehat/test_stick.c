#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <err.h>
#include <unistd.h>
#include "joy.h"

joy_t joy[1];

int
main(void)
{
	char **devs;
	size_t i, len;
	int32_t js;
	int rv;

	if (joydevs(&devs, &len) < 0)
		errx(1, "no sensehat joystick detected");

	if ((rv = joyopen(joy, devs[0]) < 0))
		errx(1, "failed to open sensehat joystick device: %s\n", strerror(-rv));

	for (;;) {
		js = joyread(joy);
		if (js <= 0)
			continue;

		if (js & JLEFT)
			printf("left ");
		if (js & JRIGHT)
			printf("right ");
		if (js & JUP)
			printf("up ");
		if (js & JDOWN)
			printf("down ");
		if (js & JENTER)
			printf("enter ");

		if (js & JPRESSED)
			printf("pressed ");
		if (js & JRELEASED)
			printf("released ");
		if (js & JHELD)
			printf("held ");
		printf("\n");
	}

	for (i = 0; i < len; i++)
		free(devs[i]);
	free(devs);
	joyclose(joy);

	return 0;
}
