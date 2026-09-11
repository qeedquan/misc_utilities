/*

https://developer.mozilla.org/en-US/docs/Web/CSS/Reference/Values/gradient/linear-gradient
https://developer.mozilla.org/en-US/docs/Web/CSS/Reference/Values/named-color
https://cssgradient.io/
https://patrickbrosset.medium.com/do-you-really-understand-css-linear-gradients-631d9a895caf

*/

#include <cstdio>
#include <cmath>
#include <vector>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <getopt.h>
#include <err.h>

using namespace std;
using namespace glm;

struct Flags
{
	int width;
	int height;
};

struct ColorStop
{
	int mode;
	float position;
	vec4 color;
};

struct LinearGradient
{
	float degrees;
	vector<ColorStop> stops;

	void load(const char *name);
	vec4 sample(float x, float y, float width, float height);
};

SDL_Window *window;
SDL_Renderer *renderer;
Flags flags;
LinearGradient lg;

vec4 parse_color(const char *color)
{
	if (!strcmp(color, "black"))
		return { 0.000, 0.000, 0.000, 1.000 };
	if (!strcmp(color, "silver"))
		return { 0.753, 0.753, 0.753, 1.000 };
	if (!strcmp(color, "gray"))
		return { 0.502, 0.502, 0.502, 1.000 };
	if (!strcmp(color, "white"))
		return { 1.000, 1.000, 1.000, 1.000 };
	if (!strcmp(color, "maroon"))
		return { 0.502, 0.000, 0.000, 1.000 };
	if (!strcmp(color, "red"))
		return { 1.000, 0.000, 0.000, 1.000 };
	if (!strcmp(color, "purple"))
		return { 0.502, 0.000, 0.502, 1.000 };
	if (!strcmp(color, "fuchsia"))
		return { 1.000, 0.000, 1.000, 1.000 };
	if (!strcmp(color, "green"))
		return { 0.000, 0.502, 0.000, 1.000 };
	if (!strcmp(color, "lime"))
		return { 0.000, 1.000, 0.000, 1.000 };
	if (!strcmp(color, "olive"))
		return { 0.502, 0.502, 0.000, 1.000 };
	if (!strcmp(color, "yellow"))
		return { 1.000, 1.000, 0.000, 1.000 };
	if (!strcmp(color, "navy"))
		return { 0.000, 0.000, 0.502, 1.000 };
	if (!strcmp(color, "blue"))
		return { 0.000, 0.000, 1.000, 1.000 };
	if (!strcmp(color, "teal"))
		return { 0.000, 0.502, 0.502, 1.000 };
	if (!strcmp(color, "aqua"))
		return { 0.000, 1.000, 1.000, 1.000 };
	if (!strcmp(color, "aliceblue"))
		return { 0.941, 0.973, 1.000, 1.000 };
	if (!strcmp(color, "antiquewhite"))
		return { 0.980, 0.922, 0.843, 1.000 };
	if (!strcmp(color, "aqua"))
		return { 0.000, 1.000, 1.000, 1.000 };
	if (!strcmp(color, "aquamarine"))
		return { 0.498, 1.000, 0.831, 1.000 };
	if (!strcmp(color, "azure"))
		return { 0.941, 1.000, 1.000, 1.000 };
	if (!strcmp(color, "beige"))
		return { 0.961, 0.961, 0.863, 1.000 };
	if (!strcmp(color, "bisque"))
		return { 1.000, 0.894, 0.769, 1.000 };
	if (!strcmp(color, "black"))
		return { 0.000, 0.000, 0.000, 1.000 };
	if (!strcmp(color, "blanchedalmond"))
		return { 1.000, 0.922, 0.804, 1.000 };
	if (!strcmp(color, "blue"))
		return { 0.000, 0.000, 1.000, 1.000 };
	if (!strcmp(color, "blueviolet"))
		return { 0.541, 0.169, 0.886, 1.000 };
	if (!strcmp(color, "brown"))
		return { 0.647, 0.165, 0.165, 1.000 };
	if (!strcmp(color, "burlywood"))
		return { 0.871, 0.722, 0.529, 1.000 };
	if (!strcmp(color, "cadetblue"))
		return { 0.373, 0.620, 0.627, 1.000 };
	if (!strcmp(color, "chartreuse"))
		return { 0.498, 1.000, 0.000, 1.000 };
	if (!strcmp(color, "chocolate"))
		return { 0.824, 0.412, 0.118, 1.000 };
	if (!strcmp(color, "coral"))
		return { 1.000, 0.498, 0.314, 1.000 };
	if (!strcmp(color, "cornflowerblue"))
		return { 0.392, 0.584, 0.929, 1.000 };
	if (!strcmp(color, "cornsilk"))
		return { 1.000, 0.973, 0.863, 1.000 };
	if (!strcmp(color, "crimson"))
		return { 0.863, 0.078, 0.235, 1.000 };
	if (!strcmp(color, "cyan"))
		return { 0.000, 1.000, 1.000, 1.000 };
	if (!strcmp(color, "darkblue"))
		return { 0.000, 0.000, 0.545, 1.000 };
	if (!strcmp(color, "darkcyan"))
		return { 0.000, 0.545, 0.545, 1.000 };
	if (!strcmp(color, "darkgoldenrod"))
		return { 0.722, 0.525, 0.043, 1.000 };
	if (!strcmp(color, "darkgray"))
		return { 0.663, 0.663, 0.663, 1.000 };
	if (!strcmp(color, "darkgreen"))
		return { 0.000, 0.392, 0.000, 1.000 };
	if (!strcmp(color, "darkgrey"))
		return { 0.663, 0.663, 0.663, 1.000 };
	if (!strcmp(color, "darkkhaki"))
		return { 0.741, 0.718, 0.420, 1.000 };
	if (!strcmp(color, "darkmagenta"))
		return { 0.545, 0.000, 0.545, 1.000 };
	if (!strcmp(color, "darkolivegreen"))
		return { 0.333, 0.420, 0.184, 1.000 };
	if (!strcmp(color, "darkorange"))
		return { 1.000, 0.549, 0.000, 1.000 };
	if (!strcmp(color, "darkorchid"))
		return { 0.600, 0.196, 0.800, 1.000 };
	if (!strcmp(color, "darkred"))
		return { 0.545, 0.000, 0.000, 1.000 };
	if (!strcmp(color, "darksalmon"))
		return { 0.914, 0.588, 0.478, 1.000 };
	if (!strcmp(color, "darkseagreen"))
		return { 0.561, 0.737, 0.561, 1.000 };
	if (!strcmp(color, "darkslateblue"))
		return { 0.282, 0.239, 0.545, 1.000 };
	if (!strcmp(color, "darkslategray"))
		return { 0.184, 0.310, 0.310, 1.000 };
	if (!strcmp(color, "darkslategrey"))
		return { 0.184, 0.310, 0.310, 1.000 };
	if (!strcmp(color, "darkturquoise"))
		return { 0.000, 0.808, 0.820, 1.000 };
	if (!strcmp(color, "darkviolet"))
		return { 0.580, 0.000, 0.827, 1.000 };
	if (!strcmp(color, "deeppink"))
		return { 1.000, 0.078, 0.576, 1.000 };
	if (!strcmp(color, "deepskyblue"))
		return { 0.000, 0.749, 1.000, 1.000 };
	if (!strcmp(color, "dimgray"))
		return { 0.412, 0.412, 0.412, 1.000 };
	if (!strcmp(color, "dimgrey"))
		return { 0.412, 0.412, 0.412, 1.000 };
	if (!strcmp(color, "dodgerblue"))
		return { 0.118, 0.565, 1.000, 1.000 };
	if (!strcmp(color, "firebrick"))
		return { 0.698, 0.133, 0.133, 1.000 };
	if (!strcmp(color, "floralwhite"))
		return { 1.000, 0.980, 0.941, 1.000 };
	if (!strcmp(color, "forestgreen"))
		return { 0.133, 0.545, 0.133, 1.000 };
	if (!strcmp(color, "fuchsia"))
		return { 1.000, 0.000, 1.000, 1.000 };
	if (!strcmp(color, "gainsboro"))
		return { 0.863, 0.863, 0.863, 1.000 };
	if (!strcmp(color, "ghostwhite"))
		return { 0.973, 0.973, 1.000, 1.000 };
	if (!strcmp(color, "gold"))
		return { 1.000, 0.843, 0.000, 1.000 };
	if (!strcmp(color, "goldenrod"))
		return { 0.855, 0.647, 0.125, 1.000 };
	if (!strcmp(color, "gray"))
		return { 0.502, 0.502, 0.502, 1.000 };
	if (!strcmp(color, "green"))
		return { 0.000, 0.502, 0.000, 1.000 };
	if (!strcmp(color, "greenyellow"))
		return { 0.678, 1.000, 0.184, 1.000 };
	if (!strcmp(color, "grey"))
		return { 0.502, 0.502, 0.502, 1.000 };
	if (!strcmp(color, "honeydew"))
		return { 0.941, 1.000, 0.941, 1.000 };
	if (!strcmp(color, "hotpink"))
		return { 1.000, 0.412, 0.706, 1.000 };
	if (!strcmp(color, "indianred"))
		return { 0.804, 0.361, 0.361, 1.000 };
	if (!strcmp(color, "indigo"))
		return { 0.294, 0.000, 0.510, 1.000 };
	if (!strcmp(color, "ivory"))
		return { 1.000, 1.000, 0.941, 1.000 };
	if (!strcmp(color, "khaki"))
		return { 0.941, 0.902, 0.549, 1.000 };
	if (!strcmp(color, "lavender"))
		return { 0.902, 0.902, 0.980, 1.000 };
	if (!strcmp(color, "lavenderblush"))
		return { 1.000, 0.941, 0.961, 1.000 };
	if (!strcmp(color, "lawngreen"))
		return { 0.486, 0.988, 0.000, 1.000 };
	if (!strcmp(color, "lemonchiffon"))
		return { 1.000, 0.980, 0.804, 1.000 };
	if (!strcmp(color, "lightblue"))
		return { 0.678, 0.847, 0.902, 1.000 };
	if (!strcmp(color, "lightcoral"))
		return { 0.941, 0.502, 0.502, 1.000 };
	if (!strcmp(color, "lightcyan"))
		return { 0.878, 1.000, 1.000, 1.000 };
	if (!strcmp(color, "lightgoldenrodyellow"))
		return { 0.980, 0.980, 0.824, 1.000 };
	if (!strcmp(color, "lightgray"))
		return { 0.827, 0.827, 0.827, 1.000 };
	if (!strcmp(color, "lightgreen"))
		return { 0.565, 0.933, 0.565, 1.000 };
	if (!strcmp(color, "lightgrey"))
		return { 0.827, 0.827, 0.827, 1.000 };
	if (!strcmp(color, "lightpink"))
		return { 1.000, 0.714, 0.757, 1.000 };
	if (!strcmp(color, "lightsalmon"))
		return { 1.000, 0.627, 0.478, 1.000 };
	if (!strcmp(color, "lightseagreen"))
		return { 0.125, 0.698, 0.667, 1.000 };
	if (!strcmp(color, "lightskyblue"))
		return { 0.529, 0.808, 0.980, 1.000 };
	if (!strcmp(color, "lightslategray"))
		return { 0.467, 0.533, 0.600, 1.000 };
	if (!strcmp(color, "lightslategrey"))
		return { 0.467, 0.533, 0.600, 1.000 };
	if (!strcmp(color, "lightsteelblue"))
		return { 0.690, 0.769, 0.871, 1.000 };
	if (!strcmp(color, "lightyellow"))
		return { 1.000, 1.000, 0.878, 1.000 };
	if (!strcmp(color, "lime"))
		return { 0.000, 1.000, 0.000, 1.000 };
	if (!strcmp(color, "limegreen"))
		return { 0.196, 0.804, 0.196, 1.000 };
	if (!strcmp(color, "linen"))
		return { 0.980, 0.941, 0.902, 1.000 };
	if (!strcmp(color, "maroon"))
		return { 0.502, 0.000, 0.000, 1.000 };
	if (!strcmp(color, "mediumaquamarine"))
		return { 0.400, 0.804, 0.667, 1.000 };
	if (!strcmp(color, "mediumblue"))
		return { 0.000, 0.000, 0.804, 1.000 };
	if (!strcmp(color, "mediumorchid"))
		return { 0.729, 0.333, 0.827, 1.000 };
	if (!strcmp(color, "mediumpurple"))
		return { 0.576, 0.439, 0.859, 1.000 };
	if (!strcmp(color, "mediumseagreen"))
		return { 0.235, 0.702, 0.443, 1.000 };
	if (!strcmp(color, "mediumslateblue"))
		return { 0.482, 0.408, 0.933, 1.000 };
	if (!strcmp(color, "mediumspringgreen"))
		return { 0.000, 0.980, 0.604, 1.000 };
	if (!strcmp(color, "mediumturquoise"))
		return { 0.282, 0.820, 0.800, 1.000 };
	if (!strcmp(color, "mediumvioletred"))
		return { 0.780, 0.082, 0.522, 1.000 };
	if (!strcmp(color, "midnightblue"))
		return { 0.098, 0.098, 0.439, 1.000 };
	if (!strcmp(color, "mintcream"))
		return { 0.961, 1.000, 0.980, 1.000 };
	if (!strcmp(color, "mistyrose"))
		return { 1.000, 0.894, 0.882, 1.000 };
	if (!strcmp(color, "moccasin"))
		return { 1.000, 0.894, 0.710, 1.000 };
	if (!strcmp(color, "navajowhite"))
		return { 1.000, 0.871, 0.678, 1.000 };
	if (!strcmp(color, "navy"))
		return { 0.000, 0.000, 0.502, 1.000 };
	if (!strcmp(color, "oldlace"))
		return { 0.992, 0.961, 0.902, 1.000 };
	if (!strcmp(color, "olive"))
		return { 0.502, 0.502, 0.000, 1.000 };
	if (!strcmp(color, "olivedrab"))
		return { 0.420, 0.557, 0.137, 1.000 };
	if (!strcmp(color, "orange"))
		return { 1.000, 0.647, 0.000, 1.000 };
	if (!strcmp(color, "orangered"))
		return { 1.000, 0.271, 0.000, 1.000 };
	if (!strcmp(color, "orchid"))
		return { 0.855, 0.439, 0.839, 1.000 };
	if (!strcmp(color, "palegoldenrod"))
		return { 0.933, 0.910, 0.667, 1.000 };
	if (!strcmp(color, "palegreen"))
		return { 0.596, 0.984, 0.596, 1.000 };
	if (!strcmp(color, "paleturquoise"))
		return { 0.686, 0.933, 0.933, 1.000 };
	if (!strcmp(color, "palevioletred"))
		return { 0.859, 0.439, 0.576, 1.000 };
	if (!strcmp(color, "papayawhip"))
		return { 1.000, 0.937, 0.835, 1.000 };
	if (!strcmp(color, "peachpuff"))
		return { 1.000, 0.855, 0.725, 1.000 };
	if (!strcmp(color, "peru"))
		return { 0.804, 0.522, 0.247, 1.000 };
	if (!strcmp(color, "pink"))
		return { 1.000, 0.753, 0.796, 1.000 };
	if (!strcmp(color, "plum"))
		return { 0.867, 0.627, 0.867, 1.000 };
	if (!strcmp(color, "powderblue"))
		return { 0.690, 0.878, 0.902, 1.000 };
	if (!strcmp(color, "purple"))
		return { 0.502, 0.000, 0.502, 1.000 };
	if (!strcmp(color, "rebeccapurple"))
		return { 0.400, 0.200, 0.600, 1.000 };
	if (!strcmp(color, "red"))
		return { 1.000, 0.000, 0.000, 1.000 };
	if (!strcmp(color, "rosybrown"))
		return { 0.737, 0.561, 0.561, 1.000 };
	if (!strcmp(color, "royalblue"))
		return { 0.255, 0.412, 0.882, 1.000 };
	if (!strcmp(color, "saddlebrown"))
		return { 0.545, 0.271, 0.075, 1.000 };
	if (!strcmp(color, "salmon"))
		return { 0.980, 0.502, 0.447, 1.000 };
	if (!strcmp(color, "sandybrown"))
		return { 0.957, 0.643, 0.376, 1.000 };
	if (!strcmp(color, "seagreen"))
		return { 0.180, 0.545, 0.341, 1.000 };
	if (!strcmp(color, "seashell"))
		return { 1.000, 0.961, 0.933, 1.000 };
	if (!strcmp(color, "sienna"))
		return { 0.627, 0.322, 0.176, 1.000 };
	if (!strcmp(color, "silver"))
		return { 0.753, 0.753, 0.753, 1.000 };
	if (!strcmp(color, "skyblue"))
		return { 0.529, 0.808, 0.922, 1.000 };
	if (!strcmp(color, "slateblue"))
		return { 0.416, 0.353, 0.804, 1.000 };
	if (!strcmp(color, "slategray"))
		return { 0.439, 0.502, 0.565, 1.000 };
	if (!strcmp(color, "slategrey"))
		return { 0.439, 0.502, 0.565, 1.000 };
	if (!strcmp(color, "snow"))
		return { 1.000, 0.980, 0.980, 1.000 };
	if (!strcmp(color, "springgreen"))
		return { 0.000, 1.000, 0.498, 1.000 };
	if (!strcmp(color, "steelblue"))
		return { 0.275, 0.510, 0.706, 1.000 };
	if (!strcmp(color, "tan"))
		return { 0.824, 0.706, 0.549, 1.000 };
	if (!strcmp(color, "teal"))
		return { 0.000, 0.502, 0.502, 1.000 };
	if (!strcmp(color, "thistle"))
		return { 0.847, 0.749, 0.847, 1.000 };
	if (!strcmp(color, "tomato"))
		return { 1.000, 0.388, 0.278, 1.000 };
	if (!strcmp(color, "turquoise"))
		return { 0.251, 0.878, 0.816, 1.000 };
	if (!strcmp(color, "violet"))
		return { 0.933, 0.510, 0.933, 1.000 };
	if (!strcmp(color, "wheat"))
		return { 0.961, 0.871, 0.702, 1.000 };
	if (!strcmp(color, "white"))
		return { 1.000, 1.000, 1.000, 1.000 };
	if (!strcmp(color, "whitesmoke"))
		return { 0.961, 0.961, 0.961, 1.000 };
	if (!strcmp(color, "yellow"))
		return { 1.000, 1.000, 0.000, 1.000 };
	if (!strcmp(color, "yellowgreen"))
		return { 0.604, 0.804, 0.196, 1.000 };

	unsigned int red, green, blue;
	if (sscanf(color, "%2x%2x%2x", &red, &green, &blue) == 3)
		return { red / 255.0, green / 255.0, blue / 255.0, 1 };

	vec4 rgba;
	if (sscanf(color, "{%f, %f, %f, %f}", &rgba.r, &rgba.y, &rgba.z, &rgba.w) == 4)
		return rgba;

	return { 0, 0, 0, 1 };
}

void LinearGradient::load(const char *name)
{
	auto fp = fopen(name, "rb");
	if (!fp)
		err(1, "open");

	stops.clear();

	char line[128];
	while (fgets(line, sizeof(line), fp))
	{
		char mode;
		float position;
		char color[64];

		if (sscanf(line, "%f", &degrees) == 1)
			continue;

		if (sscanf(line, "%c %f %63s", &mode, &position, color) != 3)
			continue;

		auto stop = ColorStop{ mode, position, parse_color(color) };
		stops.push_back(stop);
	}
	fclose(fp);
}

/*

Given a point in a rectangle (0,0)-(w,h) (screen space)
We want to map the point to the gradient line that ranges from [0, 1]
and then figure out which color stop that point belongs to and return that color

There is an angle parameter that rotates the gradient line to give more color patterns

There are a few modes that determines what color to return after the mapping occurs:
striped: no linear interpolation, return the color in the color stop as is
continuous: linear interpolation between the color stop the point falls in and the previous stop

*/
vec4 LinearGradient::sample(float x, float y, float width, float height)
{
	if (stops.empty())
		return { 0, 0, 0, 0 };
	if (stops.size() == 1)
		return stops[0].color;

	// construct a unit circle from the angle, whether or not we use sin or cos
	// depends on the orientation we want, there are alot of conventions
	// most common one for what the angle represents on the unit circle is what css uses
	float rad = radians(degrees);
	float dx = sin(rad);
	float dy = -cos(rad);

	// get the center coordinates
	float cx = width * 0.5f;
	float cy = height * 0.5f;

	// offset from center
	float px = x - cx;
	float py = y - cy;

	// (px, py) at this point represents a vector
	// apply scalar projection to the vector onto the gradient direction vector
	// this maps the coordinate point to somewhere on the gradient line (it is outside of [0, 1] however)
	float projection = px * dx + py * dy;

	// since the projection calculated above is not normalized, we need to normalize it to be in [0, 1] range
	// calculate the maximum possible projection length for normalization across the box diagonal
	float maxdist = (fabs(width * dx) + fabs(height * dy)) * 0.5;
	float t = projection / (2 * maxdist) + 0.5f;
	t = clamp(t, 0.0f, 1.0f);

	// Find surrounding color stops
	if (t <= stops.front().position)
		return stops.front().color;
	if (t >= stops.back().position)
		return stops.back().color;

	for (size_t i = 0; i < stops.size() - 1; ++i)
	{
		if (t >= stops[i].position && t <= stops[i + 1].position)
		{
			if (stops[i + 1].mode == 's')
				return stops[i + 1].color;

			float localT = (t - stops[i].position) / (stops[i + 1].position - stops[i].position);
			return mix(stops[i].color, stops[i + 1].color, localT);
		}
	}

	return stops.back().color;
}

void usage()
{
	fprintf(stderr, "usage: [options] colorfile\n");
	fprintf(stderr, "  -h    specify height (default: %d)\n", flags.height);
	fprintf(stderr, "  -w    specify width (default: %d)\n", flags.width);
	exit(2);
}

void parse_flags(int *argc, char ***argv)
{
	flags.width = 512;
	flags.height = 512;

	int opt;
	while ((opt = getopt(*argc, *argv, "w:h:")) != -1)
	{
		switch (opt)
		{
		case 'w':
			flags.width = atoi(optarg);
			break;

		case 'h':
			flags.height = atoi(optarg);
			break;
		}
	}

	*argc -= optind;
	*argv += optind;
	if (*argc < 1)
		usage();
}

void init_sdl()
{
	if (!SDL_Init(SDL_INIT_VIDEO))
		errx(1, "Failed to init SDL: %s", SDL_GetError());

	if (!SDL_CreateWindowAndRenderer("Linear Gradient", flags.width, flags.height, SDL_WINDOW_RESIZABLE, &window, &renderer))
		errx(1, "Failed to create window: %s", SDL_GetError());
}

void event()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_EVENT_QUIT:
			exit(0);
		case SDL_EVENT_KEY_DOWN:
			switch (event.key.key)
			{
			case SDLK_ESCAPE:
				exit(0);
			case SDLK_LEFT:
				lg.degrees -= 1;
				break;
			case SDLK_RIGHT:
				lg.degrees += 1;
				break;
			}
		}
	}
	lg.degrees = clamp(lg.degrees, 0.0f, 360.0f);
}

void draw_linear_gradient()
{
	int width, height;
	SDL_GetCurrentRenderOutputSize(renderer, &width, &height);

	for (float y = 0; y < height; y++)
	{
		for (float x = 0; x < width; x++)
		{
			auto color = lg.sample(x, y, width, height);
			SDL_SetRenderDrawColorFloat(renderer, color.x, color.y, color.z, color.w);
			SDL_RenderPoint(renderer, x, y);
		}
	}
}

void draw_text()
{
	char str[128];
	snprintf(str, sizeof(str), "Angle: %.3f", lg.degrees);

	float text_width = strlen(str) * 8;
	SDL_FRect rect = { 8, 8, text_width, 8 };
	SDL_SetRenderDrawColorFloat(renderer, 0, 0, 0, 1);
	SDL_RenderFillRect(renderer, &rect);

	SDL_SetRenderDrawColorFloat(renderer, 1, 1, 1, 1);
	SDL_RenderDebugText(renderer, 8, 8, str);
}

void draw()
{
	SDL_SetRenderDrawColorFloat(renderer, 0, 0, 0, 1);
	SDL_RenderClear(renderer);
	draw_linear_gradient();
	draw_text();
	SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[])
{
	parse_flags(&argc, &argv);
	init_sdl();
	lg.load(argv[0]);
	for (;;)
	{
		event();
		draw();
	}
	return 0;
}
