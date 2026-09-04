// https://en.wikipedia.org/wiki/Cohen%E2%80%93Sutherland_algorithm
// https://en.wikipedia.org/wiki/Liang%E2%80%93Barsky_algorithm

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <SDL.h>

typedef struct {
	int xmin, ymin;
	int xmax, ymax;
	SDL_Color col;
} Rect;

typedef struct {
	int x0, y0;
	int x1, y1;
	SDL_Color col;
} Line;

typedef struct {
	char desc[80];
	int (*func)(Rect, Line, Line *);
} Clipper;

enum {
	MAXSHAPES = 1000,
};

void fatal(const char *, ...);
void reset(void);
void initsdl(void);
void event(void);
void draw(void);
SDL_Color randrgb(void);
Rect rectcanon(Rect);
SDL_Rect rectsdl(Rect);
Line linecanon(Line);
int csclip(Rect, Line, Line *);
int lbclip(Rect, Line, Line *);

#define nelem(x) (sizeof(x) / sizeof(x[0]))

Clipper clippers[] = {
    {"None", NULL},
    {"Cohen-Sutherland", csclip},
    {"Liang-Barsky", lbclip},
};
size_t clipidx;

SDL_Window *window;
SDL_Renderer *renderer;

Rect rects[MAXSHAPES];
size_t nrects;

Line lines[MAXSHAPES];
size_t nlines;

Rect grabr;
Line grabl;
int grabs;

int
main(void)
{
	srand(time(NULL));
	setbuf(stdout, NULL);
	initsdl();
	reset();
	for (;;) {
		event();
		draw();
	}
	return 0;
}

void
fatal(const char *fmt, ...)
{
	char buf[1024];

	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "%s", buf);
	exit(1);
}

void
initsdl(void)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		fatal("failed to init sdl: %s", SDL_GetError());

	if (SDL_CreateWindowAndRenderer(800, 800, SDL_WINDOW_RESIZABLE, &window, &renderer) < 0)
		fatal("failed to create sdl window: %s", SDL_GetError());

	SDL_SetWindowTitle(window, "Line Clipping");
}

void
reset(void)
{
	nrects = 0;
	nlines = 0;
	clipidx = 1;
	printf("Clipper: %s\n", clippers[clipidx].desc);
}

void
event(void)
{
	SDL_Event ev;

	while (SDL_PollEvent(&ev)) {
		switch (ev.type) {
		case SDL_QUIT:
			exit(0);
		case SDL_KEYDOWN:
			switch (ev.key.keysym.sym) {
			case SDLK_ESCAPE:
				exit(0);
			case SDLK_SPACE:
				reset();
				break;
			case SDLK_LEFT:
				if (clipidx > 0)
					printf("Clipper: %s\n", clippers[--clipidx].desc);
				break;
			case SDLK_RIGHT:
				if (clipidx + 1 < nelem(clippers))
					printf("Clipper: %s\n", clippers[++clipidx].desc);
				break;
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			switch (ev.button.button) {
			case 1:
				grabr = (Rect){
				    .xmin = ev.button.x,
				    .ymin = ev.button.y,
				    .xmax = ev.button.x,
				    .ymax = ev.button.y,
				    .col = randrgb(),
				};
				grabs = 1;
				break;
			case 3:
				grabl = (Line){
				    .x0 = ev.button.x,
				    .y0 = ev.button.y,
				    .x1 = ev.button.x,
				    .y1 = ev.button.y,
				    .col = randrgb(),
				};
				grabs = 2;
				break;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			switch (grabs) {
			case 1:
				if (nrects < MAXSHAPES)
					rects[nrects++] = rectcanon(grabr);
				break;
			case 2:
				if (nlines < MAXSHAPES)
					lines[nlines++] = linecanon(grabl);
				break;
			}
			grabs = 0;
			break;
		case SDL_MOUSEMOTION:
			switch (grabs) {
			case 1:
				grabr.xmax = ev.button.x;
				grabr.ymax = ev.button.y;
				break;
			case 2:
				grabl.x1 = ev.button.x;
				grabl.y1 = ev.button.y;
				break;
			}
			break;
		}
	}
}

void
drawline(Line l)
{
	SDL_Color c;

	c = l.col;
	SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, 255);
	SDL_RenderDrawLine(renderer, l.x0, l.y0, l.x1, l.y1);
}

void
drawrect(Rect p)
{
	SDL_Rect r;
	SDL_Color c;

	r = rectsdl(p);
	c = p.col;
	SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, 255);
	SDL_RenderDrawRect(renderer, &r);
}

void
draw(void)
{
	size_t i, j;
	Line l;

	SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
	SDL_RenderClear(renderer);

	for (i = 0; i < nlines; i++) {
		if (clippers[clipidx].func && nrects) {
			for (j = 0; j < nrects; j++) {
				if (clippers[clipidx].func(rects[j], lines[i], &l))
					drawline(l);
			}
		} else
			drawline(lines[i]);
	}

	for (i = 0; i < nrects; i++)
		drawrect(rects[i]);

	switch (grabs) {
	case 1:
		drawrect(grabr);
		break;
	case 2:
		drawline(grabl);
		break;
	}

	SDL_RenderPresent(renderer);
}

void
swapi(int *x, int *y)
{
	int t;

	t = *x;
	*x = *y;
	*y = t;
}

SDL_Rect
rectsdl(Rect r)
{
	return (SDL_Rect){r.xmin, r.ymin, r.xmax - r.xmin, r.ymax - r.ymin};
}

Rect
rectcanon(Rect r)
{
	if (r.xmin > r.xmax)
		swapi(&r.xmin, &r.xmax);
	if (r.ymin > r.ymax)
		swapi(&r.ymin, &r.ymax);
	return r;
}

Line
linecanon(Line l)
{
	if (l.x0 > l.x1) {
		swapi(&l.x0, &l.x1);
		swapi(&l.y0, &l.y1);
	}
	return l;
}

SDL_Color
randrgb(void)
{
	return (SDL_Color){rand() & 0xff, rand() & 0xff, rand() & 0xff, 255};
}

int
outcode(int xmin, int ymin, int xmax, int ymax, int x, int y)
{
	int c;

	c = 0;
	if (x < xmin)
		c |= 0x1;
	else if (x > xmax)
		c |= 0x2;

	if (y < ymin)
		c |= 0x4;
	else if (y > ymax)
		c |= 0x8;

	return c;
}

int
csclip(Rect r, Line l, Line *p)
{
	int xmin, ymin, xmax, ymax;
	int x0, y0, x1, y1;
	int c0, c1, co;
	int x, y;
	int accept;

	x0 = l.x0;
	y0 = l.y0;
	x1 = l.x1;
	y1 = l.y1;
	xmin = r.xmin;
	ymin = r.ymin;
	xmax = r.xmax;
	ymax = r.ymax;

	accept = 0;
	c0 = outcode(xmin, ymin, xmax, ymax, x0, y0);
	c1 = outcode(xmin, ymin, xmax, ymax, x1, y1);
	for (;;) {
		// both points inside window, trivially accept
		if (!(c0 | c1)) {
			accept = 1;
			break;
		}

		// both points share an outside zone so
		// must be outside of window, reject
		if (c0 & c1)
			break;

		// failed both tests, so calculate line segment
		// to clip from an outside point to an intersection
		// with clip edge

		// at least one endpoint is outside the clip rectangle
		// so pick that one
		co = c0 ? c0 : c1;

		// find the intersection point;
		// use formulas:
		//   slope = (y1 - y0) / (x1 - x0)
		//   x = x0 + (1 / slope) * (ym - y0), where ym is ymin or ymax
		//   y = y0 + slope * (xm - x0), where xm is xmin or xmax
		// no need to worry about divide-by-zero because, in each case, the
		// outcode bit being tested guarantees the denominator is non-zero

		if (co & 0x8) {
			// point is above clip
			x = l.x0 + (l.x1 - l.x0) * (r.ymax - l.y0) / (l.y1 - l.y0);
			y = ymax;
		} else if (co & 0x4) {
			// point is below the clip window
			x = x0 + (x1 - x0) * (ymin - y0) / (y1 - y0);
			y = ymin;
		} else if (co & 0x2) {
			// point is to the right of clip window
			y = y0 + (y1 - y0) * (xmax - x0) / (x1 - x0);
			x = xmax;
		} else if (co & 0x1) {
			// point is to the left of clip window
			y = y0 + (y1 - y0) * (xmin - x0) / (x1 - x0);
			x = xmin;
		}

		// now we move outside point to intersection point to clip
		// and get ready for next pass.
		if (co == c0) {
			x0 = x;
			y0 = y;
			c0 = outcode(xmin, ymin, xmax, ymax, x0, y0);
		} else {
			x1 = x;
			y1 = y;
			c1 = outcode(xmin, ymin, xmax, ymax, x1, y1);
		}
	}

	*p = l;
	p->x0 = x0;
	p->x1 = x1;
	p->y0 = y0;
	p->y1 = y1;
	return accept;
}

float
maxi(float *arr, size_t n)
{
	size_t i;
	float m;

	m = 0;
	for (i = 0; i < n; i++) {
		if (m < arr[i])
			m = arr[i];
	}
	return m;
}

float
mini(float *arr, size_t n)
{
	size_t i;
	float m;

	m = 1;
	for (i = 0; i < n; i++) {
		if (m > arr[i])
			m = arr[i];
	}
	return m;
}

int
lbclip(Rect r, Line l, Line *p)
{
	float xmin, xmax, ymin, ymax;
	float x1, x2, y1, y2;
	float p1, p2, p3, p4;
	float q1, q2, q3, q4;
	float posarr[5], negarr[5];
	float r1, r2, r3, r4;
	float rn1, rn2;
	float xn1, xn2, yn1, yn2;
	size_t posind, negind;

	*p = l;

	x1 = l.x0;
	x2 = l.x1;
	y1 = l.y0;
	y2 = l.y1;
	xmin = r.xmin;
	ymin = r.ymin;
	xmax = r.xmax;
	ymax = r.ymax;

	p1 = -(x2 - x1);
	p2 = -p1;
	p3 = -(y2 - y1);
	p4 = -p3;

	q1 = x1 - xmin;
	q2 = xmax - x1;
	q3 = y1 - ymin;
	q4 = ymax - y1;

	posind = 1;
	negind = 1;
	posarr[0] = 1;
	negarr[0] = 0;
	if ((p1 == 0 && q1 < 0) || (p3 == 0 && q3 < 0))
		return 0;

	if (p1 != 0) {
		r1 = q1 / p1;
		r2 = q2 / p2;
		if (p1 < 0) {
			negarr[negind++] = r1;
			posarr[posind++] = r2;
		} else {
			negarr[negind++] = r2;
			posarr[posind++] = r1;
		}
	}
	if (p3 != 0) {
		r3 = q3 / p3;
		r4 = q4 / p4;
		if (p3 < 0) {
			negarr[negind++] = r3;
			posarr[posind++] = r4;
		} else {
			negarr[negind++] = r4;
			posarr[posind++] = r3;
		}
	}

	rn1 = maxi(negarr, negind);
	rn2 = mini(posarr, posind);

	if (rn1 > rn2)
		return 0;

	xn1 = x1 + p2 * rn1;
	yn1 = y1 + p4 * rn1;

	xn2 = x1 + p2 * rn2;
	yn2 = y1 + p4 * rn2;

	p->x0 = xn1;
	p->y0 = yn1;
	p->x1 = xn2;
	p->y1 = yn2;

	return 1;
}
