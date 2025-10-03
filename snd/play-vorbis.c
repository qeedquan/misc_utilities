#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "stb_vorbis.c"

SDL_AudioStream *stream;
short *audio;

SDL_AppResult
SDL_AppInit(void **, int argc, char *argv[])
{
	SDL_AudioSpec spec;
	int samples;

	if (argc < 2) {
		SDL_Log("usage: song.ogg");
		return SDL_APP_FAILURE;
	}

	memset(&spec, 0, sizeof(spec));
	spec.format = SDL_AUDIO_S16;
	if ((samples = stb_vorbis_decode_filename(argv[1], &spec.channels, &spec.freq, &audio)) < 0) {
		SDL_Log("Failed to load audio file");
		return SDL_APP_FAILURE;
	}

	if (!SDL_Init(SDL_INIT_AUDIO)) {
		SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
	if (!stream) {
		SDL_Log("Couldn't create audio stream: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	SDL_ResumeAudioStreamDevice(stream);
	if (!SDL_PutAudioStreamData(stream, audio, samples * spec.channels * sizeof(*audio))) {
		SDL_Log("Could not play the audio stream: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}
	return SDL_APP_CONTINUE;
}

SDL_AppResult
SDL_AppEvent(void *, SDL_Event *event)
{
	if (event->type == SDL_EVENT_QUIT) {
		return SDL_APP_SUCCESS;
	}
	return SDL_APP_CONTINUE;
}

SDL_AppResult
SDL_AppIterate(void *)
{
	if (SDL_GetAudioStreamQueued(stream) == 0)
		return SDL_APP_SUCCESS;
	return SDL_APP_CONTINUE;
}

void
SDL_AppQuit(void *, SDL_AppResult)
{
}
