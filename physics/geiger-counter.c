/*

Ported from https://tomroelandts.com/articles/simulating-a-geiger-counter

The radioactive decay events is poisson distributed, meaning the time between each event is exponentially distributed.
We can create a exponential distribution using a uniform distribution using the formula:
T = -ln(U) / L
U = uniform distribution to draw from, use formula (1-U) below to avoid encountering 0 since log(0) blows up
L = the rate parameter, the number of events per second

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <SDL.h>

static const uint8_t wav[54];

uint8_t *snd;
uint32_t sndlen;

void
fatal(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fprintf(stderr, "\n");
	exit(1);
}

double
exprand(double lambda)
{
	double u;

	u = rand() * 1.0 / RAND_MAX;
	return -log(1 - u) / lambda;
}

void
initsdl(void)
{
	SDL_AudioSpec spec;
	SDL_RWops *rw;

	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0)
		fatal("Failed to init SDL: %s", SDL_GetError());

	rw = SDL_RWFromConstMem(wav, sizeof(wav));
	if (!rw)
		fatal("Failed to create audio from memory: %s", SDL_GetError());

	if (SDL_LoadWAV_RW(rw, 1, &spec, &snd, &sndlen) == NULL)
		fatal("Failed to load WAV: %s", SDL_GetError());

	if (SDL_OpenAudio(&spec, NULL) < 0)
		fatal("Failed to open audio device: %s", SDL_GetError());

	SDL_PauseAudio(0);
}

int
main(int argc, char *argv[])
{
	uint64_t start, now;
	double t, dt;

	double lambda;
	double delay;
	unsigned tick;

	lambda = 2;
	if (argc >= 2)
		lambda = atof(argv[1]);

	srand(time(NULL));
	initsdl();

	start = SDL_GetPerformanceCounter();
	t = 0;
	for (tick = 1;; tick++) {
		now = SDL_GetPerformanceCounter();
		dt = (now - start) * 1.0 / SDL_GetPerformanceFrequency();
		t += dt;
		start = now;

		printf("%f %u\n", t, tick);

		SDL_QueueAudio(1, snd, sndlen);

		delay = exprand(lambda) * 1000;
		SDL_Delay(delay);
	}
	return 0;
}

static const uint8_t wav[54] = {
	0x52, 0x49, 0x46, 0x46, 0x7c, 0x00, 0x00, 0x00,
	0x57, 0x41, 0x56, 0x45, 0x66, 0x6d, 0x74, 0x20,
	0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00,
	0x44, 0xac, 0x00, 0x00, 0x88, 0x58, 0x01, 0x00,
	0x02, 0x00, 0x10, 0x00, 0x64, 0x61, 0x74, 0x61,
	0x58, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4b, 0x00,
	0x29, 0x01, 0x65, 0x02, 0x8a, 0x03
};
