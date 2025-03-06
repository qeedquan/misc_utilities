#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <SDL.h>

typedef struct {
	SDL_AudioSpec spec;
	Uint8 *sound;
	Uint32 soundlen;
} wave_t;

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
dumpspec(SDL_AudioSpec *spec)
{
	printf("freq     %d\n", spec->freq);
	printf("channels %d\n", spec->channels);
	printf("silence  %d\n", spec->silence);
	printf("samples  %d\n", spec->samples);
	printf("size     %d\n", spec->size);
}

void
usage(void)
{
	fprintf(stderr, "usage: [-l] file\n");
	fprintf(stderr, "  -l: loop playback\n");
	exit(2);
}

void
initsdl(void)
{
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
		fatal("Failed to init SDL: %s\n", SDL_GetError());
}

void
play(wave_t *wave)
{
	int r;

	r = SDL_QueueAudio(1, wave->sound, wave->soundlen);
	if (r != 0)
		printf("audio: %s\n", SDL_GetError());
}

int
main(int argc, char *argv[])
{
	wave_t wave;
	Uint32 size;
	Uint64 t0, t1;
	int i, j, loop;

	loop = 0;
	for (i = 1; i < argc; i++) {
		if (argv[i][0] != '-' || i + 1 >= argc)
			break;

		for (j = 1; argv[i][j]; j++) {
			switch (argv[i][j]) {
			case 'l':
				loop = 1;
				break;
			default:
				usage();
				break;
			}
		}
	}
	argc -= i;
	argv += i;

	if (argc < 1)
		usage();

	initsdl();
	if (SDL_LoadWAV(argv[0], &wave.spec, &wave.sound, &wave.soundlen) == NULL)
		fatal("Failed to load wave file: %s", SDL_GetError());
	wave.spec.callback = NULL;
	dumpspec(&wave.spec);

	if (SDL_OpenAudio(&wave.spec, NULL) < 0)
		fatal("Failed to open audio device: %s", SDL_GetError());

	SDL_PauseAudio(0);
	play(&wave);

	t0 = SDL_GetPerformanceCounter();
	for (;;) {
		size = SDL_GetQueuedAudioSize(1);
		t1 = SDL_GetPerformanceCounter();
		printf("%f: %f\n",
		       (1.0 * (t1 - t0)) / SDL_GetPerformanceFrequency(),
		       1.0 - size * 1.0 / wave.soundlen);
		if (size == 0) {
			if (loop)
				play(&wave);
			else
				break;
		}
		SDL_Delay(100);
	}

	return 0;
}
