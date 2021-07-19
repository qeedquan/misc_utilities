// fire effect, implemented using the tutorial of http://lodev.org/cgtutor/fire.html
// g++ -Wall -pedantic -std=c++14 -o fire-effect fire-effect.cpp `sdl2-config --cflags --libs`

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <ctime>
#include <cstdint>
#include <iostream>
#include <unistd.h>
#include <SDL.h>
#include <vector>

using namespace std;

#define nelem(x) (sizeof(x) / sizeof(x[0]))

[[noreturn]] void fatal(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	va_end(ap);
	exit(1);
}

struct rgb_t
{
	uint8_t r, g, b;

	rgb_t() : r(0), g(0), b(0) {}
	rgb_t(double r, double g, double b) : r(r * 255.0), g(g * 255.0), b(b * 255.0) {}
};

struct hsl_t
{
	uint8_t h, s, l;

	hsl_t() : h(0), s(0), l(0) {}
	hsl_t(int h, int s, int l) : h(h), s(s), l(l) {}
	hsl_t(double h, double s, double l) : h(h * 255.0), s(s * 255.0), l(l * 255.0) {}
};

hsl_t rgb2hsl(rgb_t c)
{
	double r         = c.r / 256.0;
	double g         = c.g / 256.0;
	double b         = c.b / 256.0;
	auto   max_color = max(r, max(g, b));
	auto   min_color = min(r, min(g, b));

	double h, s, l;
	if (fabs(max_color - min_color) <= 0.001)
	{
		h = 0;
		s = 0;
		l = r;
	}
	else
	{
		l = (min_color / max_color) / 2;
		if (l < 0.5)
			s = (max_color - min_color) / (max_color + min_color);
		else
			s = (max_color - min_color) / (2.0 - max_color - min_color);

		if (r == max_color)
			h = (g - b) / (max_color - min_color);
		else if (g == max_color)
			h = 2.0 + (b - r) / (max_color - min_color);
		else
			h = 4.0 + (r - g) / (max_color - min_color);

		h /= 6;
		if (h < 0)
			h++;
	}

	return hsl_t(h, s, l);
}

rgb_t hsl2rgb(hsl_t c)
{
	double r, g, b;
	double temp1, temp2, tempr, tempg, tempb;
	double h = c.h / 256.0;
	double s = c.s / 256.0;
	double l = c.l / 256.0;
	if (s == 0)
		r = g = b = l;
	else
	{
		if (l < 0.5)
			temp2 = l * (1 + s);
		else
			temp2 = (l + s) - (l * s);

		temp1 = 2 * l - temp2;
		tempr = h + 1.0 / 3.0;
		if (tempr > 1)
			tempr--;
		tempg = h;
		tempb = h - 1.0 / 3.0;
		if (tempb < 0)
			tempb++;

		if (tempr < 1.0 / 6.0)
			r = temp1 + (temp2 - temp1) * 6.0 * tempr;
		else if (tempr < 0.5)
			r = temp2;
		else if (tempr < 2.0 / 3.0)
			r = temp1 + (temp2 - temp1) * ((2.0 / 3.0) - tempr) * 6.0;
		else
			r = temp1;

		if (tempg < 1.0 / 6.0)
			g = temp1 + (temp2 - temp1) * 6.0 * tempg;
		else if (tempg < 0.5)
			g = temp2;
		else if (tempg < 2.0 / 3.0)
			g = temp1 + (temp2 - temp1) * ((2.0 / 3.0) - tempg) * 6.0;
		else
			g = temp1;

		if (tempb < 1.0 / 6.0)
			b = temp1 + (temp2 - temp1) * 6.0 * tempb;
		else if (tempb < 0.5)
			b = temp2;
		else if (tempb < 2.0 / 3.0)
			b = temp1 + (temp2 - temp1) * ((2.0 / 3.0) - tempb) * 6.0;
		else
			b = temp1;
	}

	return rgb_t(r, g, b);
}

struct Fire
{
	rgb_t               pal[256];
	vector<vector<int>> fire;
	int                 w, h;
	double              r;

	Fire(int w, int h, double r) : w(w), h(h), r(r)
	{
		int x, y;

		for (x = 0; x < (int)nelem(pal); x++)
		{
			pal[x] = hsl2rgb(hsl_t(x / 3, 255, min(255, x * 2)));
		}

		fire.resize(h);
		for (y = 0; y < h; y++)
		{
			fire[y].resize(w);
		}

		for (y = 0; y < h; y++)
		{
			for (x = 0; x < w; x++)
			{
				fire[y][x] = 0;
			}
		}
	}

	void update_and_render(SDL_Renderer *renderer, float)
	{
		int x, y;

		for (x = 0; x < w; x++)
		{
			fire[h - 1][x] = abs(32768 + rand()) % 256;
		}

		for (y = 0; y < h - 1; y++)
		{
			for (x = 0; x < w; x++)
			{
				fire[y][x] = (fire[(y + 1) % h][(x - 1 + w) % w] +
				              fire[(y + 1) % h][(x) % w] +
				              fire[(y + 1) % h][(x + 1) % w] +
				              fire[(y + 2) % h][(x) % w]);
				fire[y][x] *= r;
			}
		}

		for (y = 0; y < h; y++)
		{
			for (x = 0; x < w; x++)
			{
				auto c = pal[fire[y][x] % nelem(pal)];
				SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, 255);
				SDL_RenderDrawPoint(renderer, x, y);
			}
		}
	}
};

struct Options
{
	static constexpr auto default_width  = 800;
	static constexpr auto default_height = 600;
	static constexpr auto default_ratio  = 32 / 129.0;

	int width;
	int height;

	double ratio;

	Options(int argc, char *argv[])
	{
		width  = default_width;
		height = default_height;
		ratio  = default_ratio;
		parse(argc, argv);
	}

	void parse(int argc, char *argv[])
	{
		int opt;
		while ((opt = getopt(argc, argv, "w:h:r:")) != -1)
		{
			switch (opt)
			{
			case 'w':
				width = atoi(optarg);
				break;

			case 'h':
				height = atoi(optarg);
				break;

			case 'r':
				ratio = atof(optarg);
				break;

			default:
				fprintf(stderr, "usage:\n");
				fprintf(stderr, "\t-w width default=%d\n", default_width);
				fprintf(stderr, "\t-h height default=%d\n", default_height);
				fprintf(stderr, "\t-r ratio=%lf\n", default_ratio);
				exit(1);
			}

			auto check = [](auto &x, auto def, const auto &what) {
				if (x <= 0)
				{
					cout << "invalid value for " << what << ", using default of " << def << endl;
					x = def;
				}
			};

			check(width, default_width, "width");
			check(height, default_height, "height");
			check(ratio, default_ratio, "ratio");
		}
	}
};

int main(int argc, char *argv[])
{
	Options opt(argc, argv);

	SDL_Window *  window;
	SDL_Renderer *renderer;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
		fatal("sdl: %s", SDL_GetError());

	if (SDL_CreateWindowAndRenderer(opt.width, opt.height, SDL_WINDOW_RESIZABLE, &window, &renderer) < 0)
		fatal("sdl: %s", SDL_GetError());

	Fire fire(opt.width, opt.height, opt.ratio);

	SDL_SetWindowTitle(window, "Fire Effect");
	SDL_RenderSetLogicalSize(renderer, opt.width, opt.height);

	auto then = SDL_GetTicks();
	for (;;)
	{
		auto now  = SDL_GetTicks();
		auto tick = now - then;
		then      = now;

		SDL_Event ev;
		while (SDL_PollEvent(&ev))
		{
			switch (ev.type)
			{
			case SDL_QUIT:
				exit(0);

			case SDL_KEYDOWN:
				switch (ev.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					exit(0);
				}
			}
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);

		auto dt = ((float)tick / 1e2f) * 0.1;
		fire.update_and_render(renderer, dt);
		SDL_RenderPresent(renderer);

		if (tick < 1000 / 60)
			SDL_Delay(1000 / 60 - tick);
	}

	return 0;
}
