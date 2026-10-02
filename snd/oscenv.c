// based on https://github.com/OneLoneCoder/synth
#define _GNU_SOURCE
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <limits.h>
#include <math.h>
#include <time.h>
#include <getopt.h>
#include <SDL.h>

typedef struct {
	double attack;
	double decay;
	double sustain;
	double release;
	double startamp;
	double trigoff;
	double trigon;
	double noteon;
} Env;

typedef struct {
	int type;
	double freq;
} Osc;

typedef struct {
	int freq;
	int channels;
	int usecb;
	int samples;

	uint64_t start;
	uint64_t last;
	double tick;

	Uint8 aubuf[0x10000];
	size_t aubuflen;

	Osc osc[128];
	size_t nosc;

	SDL_Window *window;
	SDL_Renderer *renderer;
	Env env;
} Ctlr;

Ctlr ctlr = {
	.freq = 48000,
	.channels = 2,
	.samples = 4096,
};

enum {
	OSC_SINE = 1,
	OSC_SQUARE,
	OSC_TRIANGLE,
	OSC_SAW_ANA,
	OSC_SAW_DIG,
	OSC_NOISE
};

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

void
leput2(void *b, uint16_t v)
{
	uint8_t *p;

	p = b;
	p[0] = v & 0xff;
	p[1] = (v >> 8) & 0xff;
}

double
lerp(double t, double a, double b)
{
	return a + t * (b - a);
}

double
unlerp(double t, double a, double b)
{
	return (t - a) / (a - b);
}

double
linear_remap(double x, double a, double b, double c, double d)
{
	return lerp(unlerp(x, a, b), c, d);
}

void
envinit(Env *e)
{
	*e = (Env){
		.attack = 0.10,
		.decay = 0.01,
		.startamp = 1.0,
		.sustain = 0.80,
		.release = 0.20,
		.noteon = 0,
		.trigoff = 0.0,
		.trigon = 0.0,
	};
}

void
envnoteon(Env *e, double t)
{
	e->trigon = t;
	e->noteon = 1;
}

void
envnoteoff(Env *e, double t)
{
	e->trigoff = t;
	e->noteon = 0;
}

double
envamp(Env *e, double t)
{
	double amp, lifetime;

	lifetime = t - e->trigon;
	if (e->noteon) {
		if (lifetime <= e->attack)
			amp = (lifetime / e->attack) * e->startamp;
		else if (lifetime <= e->attack + e->decay)
			amp = ((lifetime - e->attack) / e->decay) * (e->sustain - e->startamp) + e->startamp;
		else
			amp = e->sustain;
	} else
		amp = ((t - e->trigoff) / e->release) * -e->sustain + e->sustain;

	return fmax(amp, 0.0);
}

double
osc(int type, double hz, double t)
{
	double n, v;

	switch (type) {
	case OSC_SINE:
		return sin(2.0 * M_PI * hz * t);

	case OSC_SQUARE:
		return sin(2.0 * M_PI * hz * t) > 0.0 ? 1.0 : -1.0;

	case OSC_TRIANGLE:
		return asin(sin(2 * M_PI * hz * t)) * (2.0 / M_PI);

	case OSC_SAW_ANA:
		v = 0.0;
		for (n = 1.0; n < 40.0; n++)
			v += (sin(2.0 * M_PI * n * hz * t)) / n;
		return v;

	case OSC_SAW_DIG:
		return (2.0 / M_PI) * (hz * M_PI * fmod(t, 1.0 / hz) - (M_PI / 2.0));

	case OSC_NOISE:
		return 2.0 * (drand48() - 1.0);
	}

	return 0.0;
}

void
audiofill(void *userdata, Uint8 *stream, int len)
{
	Ctlr *c;
	uint64_t now;
	double r, s;
	int16_t v;
	int i, j;
	size_t k;

	c = userdata;
	now = SDL_GetPerformanceCounter();
	printf("%f %f aubuflen %zu len %d queue %u\n",
	       1.0 * (now - c->start) / SDL_GetPerformanceFrequency(),
	       1.0 * (now - c->last) / SDL_GetPerformanceFrequency(),
	       c->aubuflen,
	       len,
	       SDL_GetQueuedAudioSize(1));
	c->last = now;

	i = 0;
	r = c->nosc;
	while (i < len) {
		s = 0;
		for (k = 0; k < c->nosc; k++)
			s += osc(c->osc[k].type, c->osc[k].freq, c->tick);
		s *= envamp(&c->env, c->tick);
		s *= 0.5;
		v = linear_remap(s, -r, r, INT16_MIN, INT16_MAX);

		for (j = 0; j < c->channels; j++) {
			if (i + 2 > len)
				break;

			leput2(stream + i, v);
			i += 2;
		}

		c->tick += 1.0 / c->freq;
	}
}

void
initsdl(void)
{
	SDL_AudioSpec spec = {
		.freq = ctlr.freq,
		.channels = ctlr.channels,
		.format = AUDIO_S16LSB,
		.samples = ctlr.samples,
		.callback = (ctlr.usecb) ? audiofill : NULL,
		.userdata = &ctlr,
	};
	SDL_AudioSpec obtained;
	SDL_Window *window;
	SDL_Renderer *renderer;

	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0)
		fatal("Failed to init SDL: %s", SDL_GetError());

	if (SDL_OpenAudio(&spec, &obtained) < 0)
		fatal("Failed to open audio device\n");

	if (SDL_CreateWindowAndRenderer(640, 480, SDL_WINDOW_RESIZABLE, &window, &renderer) < 0)
		fatal("Failed to open window: %s", SDL_GetError());

	SDL_SetWindowTitle(window, "Oscillator Envelope");

	ctlr.start = ctlr.last = SDL_GetPerformanceCounter();
	ctlr.freq = obtained.freq;
	ctlr.channels = obtained.channels;
	ctlr.samples = obtained.samples;
	ctlr.aubuflen = obtained.size;
	ctlr.window = window;
	ctlr.renderer = renderer;
	assert(ctlr.aubuflen <= sizeof(ctlr.aubuf));

	printf("Freq %d Channels %d Samples %d\n", ctlr.freq, ctlr.channels, ctlr.samples);

	SDL_PauseAudio(0);
}

void
reset(void)
{
	Ctlr *c;

	c = &ctlr;
	c->tick = 0.0;

	c->nosc = 2;
	c->osc[0] = (Osc){ OSC_SINE, 220 };
	c->osc[1] = (Osc){ OSC_SQUARE, 220 };
	envinit(&c->env);
}

void
usage(void)
{
	fprintf(stderr, "usage: [options]\n");
	fprintf(stderr, "  -u: use callback\n");
	fprintf(stderr, "  -h: show this message\n");
	exit(2);
}

void
adjustfreq(double fs)
{
	Ctlr *c;
	size_t i;

	c = &ctlr;
	for (i = 0; i < c->nosc; i++)
		c->osc[i].freq += fs;
	printf("freq %f\n", c->osc[0].freq);
}

void
event(void)
{
	Ctlr *c;
	SDL_Event ev;

	c = &ctlr;
	while (SDL_PollEvent(&ev)) {
		switch (ev.type) {
		case SDL_QUIT:
			exit(0);

		case SDL_KEYDOWN:
			switch (ev.key.keysym.sym) {
			case SDLK_ESCAPE:
				exit(0);

			case SDLK_SPACE:
				if (!c->env.noteon)
					envnoteon(&c->env, c->tick);
				break;

			case SDLK_LEFT:
				adjustfreq(-10);
				break;

			case SDLK_RIGHT:
				adjustfreq(10);
				break;
			}
			break;

		case SDL_KEYUP:
			switch (ev.key.keysym.sym) {
			case SDLK_SPACE:
				if (c->env.noteon)
					envnoteoff(&c->env, c->tick);
				break;
			}
			break;
		}
	}
}

void
update(void)
{
	Ctlr *c;
	uint64_t now;
	double dt;
	int r;

	c = &ctlr;
	if (c->usecb) {
		SDL_Delay(10);
		return;
	}

	now = SDL_GetPerformanceCounter();
	dt = 1.0 * (now - c->last) / SDL_GetPerformanceFrequency();
	if (c->last == 0 || dt >= (c->samples * 1.0) / c->freq) {
		audiofill(c, c->aubuf, c->aubuflen);
		r = SDL_QueueAudio(1, c->aubuf, c->aubuflen);
		if (r != 0)
			printf("queue audio: %s\n", SDL_GetError());
	}
}

void
draw(void)
{
	Ctlr *c;

	c = &ctlr;
	SDL_RenderClear(c->renderer);
	SDL_RenderPresent(c->renderer);
}

int
main(int argc, char *argv[])
{
	int c;

	while ((c = getopt(argc, argv, "hu")) != -1) {
		switch (c) {
		case 'u':
			ctlr.usecb = 1;
			break;

		case 'h':
		default:
			usage();
			break;
		}
	}
	argc -= optind;
	argv += optind;

	srand48(time(NULL));
	reset();
	initsdl();
	for (;;) {
		event();
		update();
		draw();
	}
	return 0;
}
