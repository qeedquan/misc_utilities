/* cc -Wall -Wextra -o check-glsl check-glsl.c `sdl2-config --cflags --libs` -lGL -lGLEW */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <SDL.h>
#include <GL/glew.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

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

void *
xmallocz(size_t size)
{
	void *p;

	if (size == 0)
		size = 1;
	p = calloc(1, size);
	if (p == NULL)
		fatal("malloc: %s", strerror(errno));
	return p;
}

void *
readfile(const char *name, size_t *len)
{
	FILE *fp;
	void *p;
	long n;
	int rv;

	fp = fopen(name, "rb");
	if (fp == NULL)
		return NULL;

	fseek(fp, 0, SEEK_END);
	n = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	if (n < 0)
		return NULL;

	p = xmallocz(n);
	rv = fread(p, n, 1, fp);
	fclose(fp);

	if ((n > 0 && rv != 1) || (n == 0 && rv != 0)) {
		free(p);
		return NULL;
	}

	if (len)
		*len = n;
	return p;
}

void *
xreadfile(const char *name, size_t *len)
{
	void *p;

	p = readfile(name, len);
	if (p == NULL)
		fatal("failed to read file: %s: %s", name, strerror(errno));
	return p;
}

void
usage(void)
{
	fprintf(stderr, "usage: [options] vertex_file fragment_file\n");
	fprintf(stderr, "  -v: gl version (default 3.3)\n");
	exit(2);
}

GLuint
compile(GLuint type, const char *name, const char *src)
{
	GLuint shader;
	GLint status, length;
	GLchar *log;

	shader = glCreateShader(type);
	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		log = xmallocz(length);

		glGetShaderInfoLog(shader, length, &length, &log[0]);
		fprintf(stderr, "%s: %s\n", name, log);

		glDeleteShader(shader);
	}
	return shader;
}

void
check(const char *vertex_file, const char *fragment_file)
{
	char *vertex_source, *fragment_source;
	GLuint program, vertex, fragment;
	GLint status, length;
	GLchar *log;

	vertex_source = xreadfile(vertex_file, NULL);
	fragment_source = xreadfile(fragment_file, NULL);

	program = glCreateProgram();
	vertex = compile(GL_VERTEX_SHADER, vertex_file, vertex_source);
	fragment = compile(GL_FRAGMENT_SHADER, fragment_file, fragment_source);

	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
		log = xmallocz(length);

		glGetProgramInfoLog(program, length, &length, &log[0]);
		fatal("%s", log);
	}
}

int
main(int argc, char *argv[])
{
	SDL_Window *window;
	GLint err;
	int gl_major, gl_minor, c;

	gl_major = 3;
	gl_minor = 3;
	while ((c = getopt(argc, argv, "v:h")) != -1) {
		switch (c) {
		case 'v':
			sscanf(optarg, "%d.%d", &gl_major, &gl_minor);
			break;
		case 'h':
			usage();
		}
	}
	argc -= optind;
	argv += optind;

	if (argc < 2)
		usage();

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		fatal("%s", SDL_GetError());

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_major);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_minor);

	window = SDL_CreateWindow("GLSL checker", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 256, 256, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN);
	if (window == NULL)
		fatal("%s", SDL_GetError());

	SDL_GL_CreateContext(window);

	err = glewInit();
	if (err != GLEW_OK)
		fatal("%s", glewGetErrorString(err));

	check(argv[0], argv[1]);

	return 0;
}
