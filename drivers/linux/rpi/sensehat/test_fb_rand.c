#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <err.h>
#include "fb.h"
#include "joy.h"
#include "util.h"

volatile sig_atomic_t interrupted;

void
interrupt(int sig)
{
	interrupted = 1;
	(void)sig;
}

fb_t fb[1];
joy_t joy[1];

void
event(void)
{
	uint8_t gamma[32];
	int32_t key;
	size_t i;

	key = joyread(joy);
	if (key <= 0 || !(key & (JPRESSED | JHELD)))
		return;

	fbgetgamma(fb, gamma);
	if (key & JDOWN) {
		printf("down\n");
		for (i = 0; i < nelem(gamma); i++)
			gamma[i]--;
		fbsetgamma(fb, gamma);
	}
	if (key & JUP) {
		printf("up\n");
		for (i = 0; i < nelem(gamma); i++)
			gamma[i]++;
		fbsetgamma(fb, gamma);
	}
	if (key & JENTER) {
		printf("enter\n");
		fbresetgamma(fb, FB_GAMMA_LOW);
	}
}

void
drawrand(void)
{
	int x, y;

	for (y = 0; y < fb->h; y++) {
		for (x = 0; x < fb->w; x++) {
			fbset(fb, x, y, fbrgb(rand(), rand(), rand()));
		}
	}
}

void
draw(void)
{
	drawrand();
}

int
main(void)
{
	char **fbd, **joyd;
	size_t nfb, njoy;
	size_t i;

	srand(time(NULL));
	signal(SIGINT, interrupt);

	if (fbdevs(&fbd, &nfb) < 0)
		errx(1, "no sensehat led devices detected");

	if (joydevs(&joyd, &njoy) < 0)
		errx(1, "no sense joystick devices detected\n");

	if (fbopen(fb, fbd[0]) < 0)
		errx(1, "failed to open sensehat led device: %s", strerror(errno));

	if (joyopen(joy, joyd[0]) < 0)
		errx(1, "failed to open sensehat joystick device: %s", strerror(errno));

	for (;;) {
		event();
		draw();
		sleep(1);
		if (interrupted)
			break;
	}

	for (i = 0; i < nfb; i++)
		free(fbd[i]);
	free(fbd);
	for (i = 0; i < njoy; i++)
		free(joyd[i]);
	free(joyd);

	fbclose(fb);
	joyclose(joy);

	return 0;
}
