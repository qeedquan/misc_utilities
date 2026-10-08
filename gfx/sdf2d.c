// ported from https://www.iquilezles.org/www/articles/distfunctions2d/distfunctions2d.htm
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <SDL.h>
#include "linalg.h"
#include "sdf.h"

#define nelem(x) (sizeof(x) / sizeof(x[0]))

typedef struct {
	uint32_t *pixels;
	int width;
	int height;
} Image;

typedef struct {
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture;

	int width, height;
	Image image;

	bool paused;
	bool running;
	int mode;

	uint64_t start;

	struct {
		float2 resolution;
		float2 aspect;
		float time;
	};
} Context;

enum {
	CIRCLE = 0,
	ROUND_RECT,
	RECT,
	ORIENTED_RECT,
	SEGMENT,
	RHOMBUS,
	EQUILATERAL_TRIANGLE,
	ISOCELES_TRIANGLE,
	UNEVEN_CAPSULE,
	TRIANGLE,
	PENTAGON,
	HEXAGON,
	OCTAGON,
	HEXAGRAM,
	STAR5,
	STAR,
	TRAPEZOID,
	PIE,
	ARC,
	HORSESHOE,
	VESICA,
	SIMPLE_EGG,
	CROSS,
	ROUNDED_X,
	POLYGON,
	ELLIPSE,
	PARABOLA,
	PARABOLA_SEGMENT,
	QUADRATIC_BEZIER,
	MAX_SHAPES,
};

bool
ispow2(intmax_t x)
{
	return (x & (x - 1)) == 0;
}

intmax_t
nextpow2(intmax_t x)
{
	intmax_t v;

	v = 1;
	while (v < x)
		v <<= 1;
	return v;
}

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
xrealloc(void *ptr, size_t size)
{
	void *p;

	size = max(size, 1);
	p = realloc(ptr, size);
	if (!p)
		abort();
	return p;
}

void *
xcalloc(size_t nmemb, size_t size)
{
	void *p;

	nmemb = max(nmemb, 1);
	size = max(size, 1);
	p = calloc(nmemb, size);
	if (!p)
		abort();
	return p;
}

void
newimage(Image *m, int w, int h)
{
	assert(ispow2(w));
	assert(ispow2(h));

	if (w * h >= m->width * m->height)
		m->pixels = xrealloc(m->pixels, 4 * w * h);

	m->width = w;
	m->height = h;
}

static inline uint32_t
rgba4(float4 v)
{
	uint32_t r, g, b, a;

	r = clampf(v.r * 255, 0, 255);
	g = clampf(v.g * 255, 0, 255);
	b = clampf(v.b * 255, 0, 255);
	a = clampf(v.a * 255, 0, 255);
	return b | g << 8 | r << 16 | a << 24;
}

static inline void
pixel(Image *m, int x, int y, uint32_t col)
{
	x &= (m->width - 1);
	y &= (m->height - 1);
	m->pixels[y * m->width + x] = col;
}

void
background(Image *m, float4 col)
{
	int i, n;
	uint32_t p;

	p = rgba4(col);
	n = m->width * m->height;
	for (i = 0; i < n; i++)
		m->pixels[i] = p;
}

void
resize(Context *c, int w, int h)
{
	int nw, nh;
	float a;

	if (w <= 0 || h <= 0)
		return;

	if (w > c->width || h > c->height) {
		nw = nextpow2(w);
		nh = nextpow2(h);
		nw = max(nw, nh);
		nh = nw;

		newimage(&c->image, nw, nh);

		if (c->texture)
			SDL_DestroyTexture(c->texture);

		c->texture = SDL_CreateTexture(c->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, nw, nh);
		if (!c->texture)
			fatal("Failed to create texture: %s", SDL_GetError());
	}

	a = min(w, h);
	c->width = w;
	c->height = h;
	c->resolution = (float2){{w, h}};
	c->aspect = (float2){{w / a, h / a}};
}

void
initsdl(Context *c)
{
	SDL_Window *win;
	SDL_Renderer *re;
	Uint32 wflag;
	int w, h;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
		fatal("Failed to init SDL: %s", SDL_GetError());

	w = 800;
	h = 600;
	wflag = SDL_WINDOW_RESIZABLE;
	if (SDL_CreateWindowAndRenderer(w, h, wflag, &win, &re) < 0)
		fatal("Failed to create a window: %s", SDL_GetError());

	SDL_SetWindowTitle(win, "2D SDF Shapes");

	c->window = win;
	c->renderer = re;
	resize(c, w, h);
}

void
reset(Context *c)
{
	c->time = 0;
	c->running = true;
	c->paused = false;
	c->start = SDL_GetPerformanceCounter();
}

void
event(Context *c)
{
	SDL_Event ev;

	while (SDL_PollEvent(&ev)) {
		switch (ev.type) {
		case SDL_QUIT:
			c->running = false;
			break;

		case SDL_KEYDOWN:
			switch (ev.key.keysym.sym) {
			case SDLK_ESCAPE:
				c->running = false;
				break;
			case SDLK_LEFT:
				c->mode--;
				break;
			case SDLK_RIGHT:
				c->mode++;
				break;
			case SDLK_SPACE:
				reset(c);
				break;
			case SDLK_RETURN:
				c->paused = !c->paused;
				break;
			}
			c->mode = wrapf(c->mode, 0, MAX_SHAPES - 1);
			break;

		case SDL_WINDOWEVENT:
			switch (ev.window.event) {
			case SDL_WINDOWEVENT_RESIZED:
			case SDL_WINDOWEVENT_SIZE_CHANGED:
				resize(c, ev.window.data1, ev.window.data2);
				break;
			}
			break;
		}
	}
}

void
upload(Context *c)
{
	Image *m;
	void *data;
	int pitch;

	m = &c->image;
	SDL_LockTexture(c->texture, NULL, &data, &pitch);
	memcpy(data, m->pixels, m->width * m->height * 4);
	SDL_UnlockTexture(c->texture);
}

// [0:width, 0:height] -> [-1, 1]
static inline float2
s2p(Context *c, float2 sp)
{
	return (float2){{
	    (2.0 * sp.x - c->resolution.x) / (c->resolution.x / c->aspect.x),
	    (2.0 * sp.y - c->resolution.y) / (c->resolution.y / c->aspect.y),
	}};
}

static inline float4
bandcolor(float d)
{
	float3 col = {{1, 1, 1}};
	col = sub3(col, scale3((float3){{0.1, 0.4, 0.7}}, signf(d)));
	col = scale3(col, 1.0 - expf(-3.0 * fabsf(d)));
	col = scale3(col, 0.8 + 0.2 * cosf(150.0 * d));
	col = lerp3(1.0 - smoothstepf(0.0, 0.02, fabsf(d)), col, (float3){{1.0, 1.0, 1.0}});
	return xyzw3(col, 1);
}

float4
shcircle(Context *c, float2 sp)
{
	float2 p = s2p(c, sp);
	float d = sdcircle(p, 0.5);
	return bandcolor(d);
}

float4
shroundrect(Context *c, float2 sp)
{
	float2 p = s2p(c, sp);
	float2 si = {{0.9, 0.6}};
	float4 ra = {{0, 1, 2, 3}};
	float t = 2 * c->time;
	ra = cos4(add4(ra, (float4){{t, t, t, t}}));
	ra = scale4(ra, 0.3);
	ra = add4(ra, (float4){{0.3, 0.3, 0.3, 0.3}});

	float d = sdroundrect(p, si, ra);
	return bandcolor(d);
}

float4
shrect(Context *c, float2 sp)
{
	float2 p = s2p(c, sp);
	float t = 2 * c->time;
	float2 b = {{0.5, 1}};
	b = cos2(add2(b, (float2){{t, t}}));
	b = scale2(b, 0.3);
	b = add2(b, (float2){{0.3, 0.3}});

	float d = sdrect(p, b);
	return bandcolor(d);
}

float4
shorientedrect(Context *c, float2 sp)
{
	float2 p = s2p(c, sp);
	float t = c->time;
	float2 b = {{0.5, 0.8}};
	b = cos2(add2(b, (float2){{t, t}}));
	b = scale2(b, 0.3);
	b = add2(b, (float2){{0.3, 0.3}});

	float d = sdorientedrect(p, b, perp2(b), 1);
	return bandcolor(d);
}

float4
shsegment(Context *c, float2 sp)
{
	float2 p = s2p(c, sp);
	p = scale2(p, 1.4);

	float t = c->time;
	float2 v1 = add2((float2){{0.0, 2.00}}, (float2){{t, t}});
	float2 v2 = add2((float2){{0.0, 1.50}}, (float2){{t + 1.5, t + 1.5}});
	v1 = cos2(v1);
	v2 = cos2(v2);
	float th = 0.1 * (0.5 + 0.5 * sin(t * 1.1));

	float d = sdsegment(p, v1, v2) - th;
	return bandcolor(d);
}

float4
shrhombus(Context *c, float2 sp)
{
	float2 p = s2p(c, sp);
	float t = c->time;
	float2 ra = cos2(add2((float2){{t, t}}, (float2){{0.0, 1.57}}));
	ra = scale2(ra, 0.3);
	ra = add2(ra, (float2){{0.4, 0.4}});

	float d = sdrhombus(p, ra);
	return bandcolor(d);
}

float4
shequtriangle(Context *c, float2 sp)
{
	float2 p = s2p(c, sp);
	float t = c->time;
	p = scale2(p, 2.0);
	p.x += 0.5 * cos(t);
	p.y += 0.5 * sin(t);

	float d = sdequtriangle(p);
	return bandcolor(d);
}

float4
shisotriangle(Context *c, float2 sp)
{
	float2 p = s2p(c, sp);
	float t = c->time;
	p = scale2(p, 1.1);
	p = sub2(p, (float2){{0, 0.5}});
	p.x += 0.5 * cos(t);
	p.y += 0.5 * sin(t);
	float2 tri = {{.3, -1.1}};

	float d = sdisotriangle(p, tri);
	return bandcolor(d);
}

float4
shunevencapsule(Context *c, float2 sp)
{
	float2 p = s2p(c, sp);
	p = scale2(p, 1.4);

	float t = c->time;
	float2 v1 = add2((float2){{0.0, 2.00}}, (float2){{t, t}});
	float2 v2 = add2((float2){{0.0, 1.50}}, (float2){{t + 1.5, t + 1.5}});
	v1 = cos2(v1);
	v2 = cos2(v2);
	float r1 = 0.5 + 0.1 * sinf(t);
	float r2 = 0.3 + 0.1 * sinf(1.0 + 2.3 * t);

	float d = sdunevencapsule(p, v1, v2, r1, r2);
	return bandcolor(d);
}

float4
shtriangle(Context *c, float2 sp)
{
	float2 p = s2p(c, sp);
	p = scale2(p, 1.5);

	float t = c->time;
	float2 v1 = add2((float2){{0.0, 2.00}}, (float2){{t, t}});
	float2 v2 = add2((float2){{0.0, 1.50}}, (float2){{t + 1.5, t + 1.5}});
	float2 v3 = add2((float2){{0.0, 3.00}}, (float2){{t + 4.0, t + 4.0}});
	v1 = cos2(v1);
	v2 = cos2(v2);
	v3 = cos2(v3);

	float d = sdtriangle(p, v1, v2, v3);
	return bandcolor(d);
}

float4
shpentagon(Context *c, float2 sp)
{
	float2 p = s2p(c, sp);
	float t = c->time;

	float d = sdpentagon(p, fabsf(cosf(t)));
	return bandcolor(d);
}

float4
shhexagon(Context *c, float2 sp)
{
	float2 p = s2p(c, sp);
	float t = c->time;
	p.x += 0.2 * cos(t);
	p.y += 0.2 * sin(t);

	float d = sdhexagon(p, fabsf(cosf(t) * sinf(t)));
	return bandcolor(d);
}

float4
shoctagon(Context *c, float2 sp)
{
	float2 p = s2p(c, sp);
	float t = c->time;
	p.x += 0.2 * cos(t);
	p.y += 0.2 * sin(t);

	float d = sdoctagon(p, fabsf(sinf(t)));
	return bandcolor(d);
}

float4
shhexagram(Context *c, float2 sp)
{
	float2 p = s2p(c, sp);
	float t = c->time;
	p.x += 0.1 * cos(t);

	float d = sdhexagram(p, 0.5 * fabsf(sinf(t)));
	return bandcolor(d);
}

float4
shstar5(Context *c, float2 sp)
{
	float2 p = s2p(c, sp);
	float t = c->time / 3.0;

	float d = sdstar5(p, fabs(cos(t)), fabs(sin(t)));
	return bandcolor(d);
}

float4
shstar(Context *c, float2 sp)
{
	float2 p = s2p(c, sp);
	float x;
	float t = c->time / 3.0;
	float n = 3.0 + fmodf(floor(t), 9.0); // n, number of sides
	float a = modff(t, &x);               // angle factor
	float m = 2.0 + a * a * (n - 2.0);    // angle divisor, between 2 and n

	float d = sdstar(p, 0.7, n, m);
	return bandcolor(d);
}

float4
shtrapezoid(Context *c, float2 sp)
{
	float2 p = s2p(c, sp);
	float t = c->time;
	float ra = 0.2 + 0.15 * sinf(t * 1.3 + 0.0);
	float rb = 0.2 + 0.15 * sinf(t * 1.4 + 1.1);
	float2 ka = scale2(sin2((float2){{0, t * 1.1 + 2.0}}), 0.4);
	float2 kb = scale2(sin2((float2){{t * 1.2 + 1.0, t * 1.2 + 2.5}}), 0.4);
	float2 pa = add2((float2){{-0.6, 0.0}}, ka);
	float2 pb = add2((float2){{-0.6, 0.0}}, kb);
	float2 pc = {{0.8, 0.0}};

	// axis aligned trapezoid and arbitrary trapezoid
	float d1 = sdtrapezoidy(sub2(p, pc), ra, rb, 0.5 + 0.2 * sinf(1.3 * t));
	float d2 = sdtrapezoid(p, pa, pb, ra, rb);
	float d = min(d1, d2);

	return bandcolor(d);
}

float4
shpie(Context *c, float2 sp)
{
	float2 p = s2p(c, sp);
	float t = M_PI * (0.5 + 0.5 * cosf(c->time * 0.52));

	float d = sdpie(p, (float2){{sinf(t), cosf(t)}}, 0.5);
	return bandcolor(d);
}

float4
sharc(Context *c, float2 sp)
{
	float2 p = s2p(c, sp);
	float t = c->time;
	float ta = M_PI * (0.5 + 0.5 * cosf(t * 0.52 + 2.0));
	float tb = M_PI * (0.5 + 0.5 * cosf(t * 0.31 + 2.0));
	float rb = 0.15 * (0.5 + 0.5 * cosf(t * 0.41 + 3.0));

	float d = sdarc(p, (float2){{sinf(ta), cosf(ta)}}, (float2){{sinf(tb), cosf(tb)}}, 0.7, rb);
	return bandcolor(d);
}

float4
shhorseshoe(Context *c, float2 sp)
{
	float2 p = s2p(c, sp);
	float t = M_PI * (0.3 + 0.3 * cos(c->time * 0.5));
	float2 k = scale2((float2){{0.7, 1.1}}, c->time);
	k = add2(k, (float2){{0.0, 2.0}});
	k = cos2(k);
	k = scale2(k, 0.5);
	k = add2(k, (float2){{0.5, 0.5}});
	float2 w = hadamard2((float2){{0.750, 0.25}}, k);

	float d = sdhorseshoe(sub2(p, (float2){{0.0, -0.1}}), (float2){{cos(t), sin(t)}}, 0.5, w);
	return bandcolor(d);
}

float4
shvesica(Context *c, float2 sp)
{
	float2 p = s2p(c, sp);
	float t = c->time;
	float ra1 = 0.5 * cosf(t + 1.0);
	float ra2 = 0.2 * sinf(t * 1.3);

	float d = sdvesica(p, 0.7, ra1) + ra2;
	return bandcolor(d);
}

float4
shegg(Context *c, float2 sp)
{
	float2 p = s2p(c, sp);
	float t = c->time;
	float ra = 0.6;
	float rb = ra * (0.55 + 0.45 * cosf(2.0 * t));

	float d = sdegg(add2(p, (float2){{0., 0.2}}), ra, rb);
	return bandcolor(d);
}

float4
shcross(Context *c, float2 sp)
{
	float2 p = s2p(c, sp);
	float t = c->time;
	// size
	float2 si = {{0.5, 0.5}};
	si = add2(si, scale2(cos2((float2){{t + 0.0, t + 1.57}}), 0.3));
	if (si.x < si.y)
		swapf(&si.x, &si.y);

	// corner radius
	float ra = 0.1 * sinf(t * 1.2);
	float d = sdcross(p, si, ra);
	return bandcolor(d);
}

float4
shroundx(Context *c, float2 sp)
{
	float2 p = s2p(c, sp);
	float t = c->time;
	float wi = 0.5 + 0.3 * cosf(t);
	float ra = 0.1 + 0.08 * sinf(t * 1.2);

	float d = sdroundx(p, wi, ra);
	return bandcolor(d);
}

float4
shpolygon(Context *c, float2 sp)
{
	float2 p = s2p(c, sp);
	float t = c->time;

	float2 v[] = {
	    {{0, 2.00}},
	    {{0, 1.50}},
	    {{0, 3.00}},
	    {{0, 2.00}},
	    {{0, 1.00}},
	};
	for (size_t i = 0; i < nelem(v); i++) {
		float2 vt = {{(0.40 + 0.05 * i) * t, (0.40 + 0.05 * i) * t}};
		float2 vo = {{i, i}};
		v[i] = add2(v[i], vt);
		v[i] = add2(v[i], vo);
		v[i] = cos2(v[i]);
		v[i] = scale2(v[i], 0.8);
	}

	float d = sdpolygon(p, v, nelem(v));
	return bandcolor(d);
}

float4
shparabola(Context *c, float2 sp)
{
	float2 p = s2p(c, sp);
	float t = c->time / 2.0;
	float px = 0.0 + 0.4 * cosf(t * 1.1 + 5.5);  // x position
	float py = -0.4 + 0.2 * cosf(t * 1.2 + 3.0); // y position
	float pk = 8.0 + 7.5 * cosf(t * 1.3 + 3.5);  // width

	float d = sdparabola(sub2(p, (float2){{px, py}}), pk);
	return bandcolor(d);
}

float4
shellipse(Context *c, float2 sp)
{
	float2 p = s2p(c, sp);
	float t = c->time;
	float r = 0.3 + cosf(t);
	float2 u1 = {{0.6, 1.0}};
	float2 u2 = {{0.4, 0.4}};
	float2 u = add2(scale2(u1, r), u2);

	float d = sdellipse(p, abs2(u));
	return bandcolor(d);
}

float4
shsegparabola(Context *c, float2 sp)
{
	float2 p = s2p(c, sp);
	float t = c->time / 2.0;
	float w = 0.7 + 0.69 * sinf(t * 0.61 + 0.0);
	float h = 0.4 + 0.35 * sinf(t * 0.53 + 2.0);

	float d = sdsegparabola(p, w, h);
	return bandcolor(d);
}

float4
shbezier2(Context *c, float2 sp)
{
	float2 p = s2p(c, sp);
	float t = c->time;

	float2 v0 = V2(1.3, 0.9);
	float2 v1 = V2(1.3, 0.9);
	float2 v2 = V2(1.3, 0.9);

	float2 t0 = V2(0, 5);
	float2 t1 = V2(3, 4);
	float2 t2 = V2(2, 1);

	t0 = add2(t0, S2(t * 0.5));
	t1 = add2(t1, S2(t * 0.6));
	t2 = add2(t2, S2(t * 0.7));

	t0 = cos2(t0);
	t1 = cos2(t1);
	t2 = cos2(t2);

	v0 = hadamard2(v0, t0);
	v1 = hadamard2(v1, t1);
	v2 = hadamard2(v2, t2);

	float d = sdbezier2(p, v0, v1, v2);
	return bandcolor(d);
}

void
shapes(Context *c)
{
	Image *m;
	uint32_t *p;
	float2 sp;
	float4 col;
	int x, y;

	m = &c->image;
	for (y = 0; y < c->height; y++) {
		p = m->pixels + (c->height - y - 1) * m->width;
		for (x = 0; x < c->width; x++) {
			sp = (float2){{x, y}};
			switch (c->mode) {
			case CIRCLE:
				col = shcircle(c, sp);
				break;
			case ROUND_RECT:
				col = shroundrect(c, sp);
				break;
			case RECT:
				col = shrect(c, sp);
				break;
			case ORIENTED_RECT:
				col = shorientedrect(c, sp);
				break;
			case SEGMENT:
				col = shsegment(c, sp);
				break;
			case RHOMBUS:
				col = shrhombus(c, sp);
				break;
			case EQUILATERAL_TRIANGLE:
				col = shequtriangle(c, sp);
				break;
			case ISOCELES_TRIANGLE:
				col = shisotriangle(c, sp);
				break;
			case UNEVEN_CAPSULE:
				col = shunevencapsule(c, sp);
				break;
			case TRIANGLE:
				col = shtriangle(c, sp);
				break;
			case PENTAGON:
				col = shpentagon(c, sp);
				break;
			case HEXAGON:
				col = shhexagon(c, sp);
				break;
			case OCTAGON:
				col = shoctagon(c, sp);
				break;
			case HEXAGRAM:
				col = shhexagram(c, sp);
				break;
			case STAR5:
				col = shstar5(c, sp);
				break;
			case STAR:
				col = shstar(c, sp);
				break;
			case TRAPEZOID:
				col = shtrapezoid(c, sp);
				break;
			case PIE:
				col = shpie(c, sp);
				break;
			case ARC:
				col = sharc(c, sp);
				break;
			case HORSESHOE:
				col = shhorseshoe(c, sp);
				break;
			case VESICA:
				col = shvesica(c, sp);
				break;
			case SIMPLE_EGG:
				col = shegg(c, sp);
				break;
			case CROSS:
				col = shcross(c, sp);
				break;
			case ROUNDED_X:
				col = shroundx(c, sp);
				break;
			case POLYGON:
				col = shpolygon(c, sp);
				break;
			case ELLIPSE:
				col = shellipse(c, sp);
				break;
			case PARABOLA:
				col = shparabola(c, sp);
				break;
			case PARABOLA_SEGMENT:
				col = shsegparabola(c, sp);
				break;
			case QUADRATIC_BEZIER:
				col = shbezier2(c, sp);
				break;
			default:
				col = (float4){{0, 0, 0, 1}};
				break;
			}
			*p++ = rgba4(col);
		}
	}
}

void
update(Context *c)
{
	uint64_t now;
	double dt;

	now = SDL_GetPerformanceCounter();
	dt = (now - c->start) * 1.0 / SDL_GetPerformanceFrequency();

	c->start = now;
	if (!c->paused)
		c->time += dt;
}

void
draw(Context *c)
{
	SDL_Renderer *re;
	SDL_Rect r;

	re = c->renderer;
	r = (SDL_Rect){0, 0, c->width, c->height};

	SDL_RenderClear(re);
	shapes(c);
	upload(c);
	SDL_RenderCopy(re, c->texture, &r, &r);
	SDL_RenderPresent(re);
}

int
main(void)
{
	Context c[1];

	memset(c, 0, sizeof(*c));
	initsdl(c);
	reset(c);
	c->mode = CIRCLE;
	while (c->running) {
		event(c);
		update(c);
		draw(c);
	}
	return 0;
}
