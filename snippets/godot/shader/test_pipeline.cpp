// https://docs.godotengine.org/en/stable/engine_details/architecture/internal_rendering_architecture.html
// https://docs.godotengine.org/en/4.3/tutorials/shaders/shader_reference/shading_language.html
#define GLM_FORCE_SWIZZLE
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <cassert>
#include <cstdio>
#include <cmath>
#include <algorithm>
#include <glm/glm.hpp>
#include <stb_image.h>
#include <stb_image_write.h>

using namespace std;
using namespace glm;

float outline_width = 3.0;


/*

It's important to understand that textures that are supplied as color require hints for proper sRGB -> linear conversion (i.e. source_color), as Godot's 3D engine renders in linear color space. If this is not done, the texture will appear washed out.

The 2D renderer also renders in linear color space if the Rendering > Viewport > HDR 2D project setting is enabled, so source_color must also be used in canvas_item shaders. If 2D HDR is disabled, source_color will keep working correctly in canvas_item shaders, so it's recommend to use it either way.

Shaders perform their operations in linear space. If you want to keep your texture in sRGB then it will have to be converted to linear in the shader. In Godot, the default workflow is that albedo textures will be in sRGB while other textures will not. This is generally a good assumption, however, it requires that users specify when the texture is an albedo texture (i.e. by using a hint).
*/

// shader colors can be outside of the standard [0, 1] range (if you apply the 2^I where I is intensity in the color picker)
vec4 outline_color = vec4(15.022, 0.571, 0.573, 1.0);

int WIDTH;
int HEIGHT;
int CHANNELS;
unsigned char *TEXTURE;
vec2 UV;
vec2 TEXTURE_PIXEL_SIZE;

vec4 texture(unsigned char *texture, vec2 uv)
{
	int x = (uv.x * WIDTH) + 0.5;
	int y = (uv.y * HEIGHT) + 0.5;
	if (x < 0)
		x = 0;
	if (y < 0)
		y = 0;
	if (x >= WIDTH)
		x = WIDTH - 1;
	if (y >= HEIGHT)
		y = HEIGHT - 1;

	unsigned char *p = &texture[(y * WIDTH + x) * CHANNELS];

	float r = p[0] / 255.0f;
	float g = p[1] / 255.0f;
	float b = p[2] / 255.0f;
	float a = p[3] / 255.0f;
	return vec4(r, g, b, a);
}

void putrgb(unsigned char *pixels, vec4 color)
{
	color = clamp(color * vec4(255), 0.0f, 255.0f);
	pixels[0] = color.r;
	pixels[1] = color.g;
	pixels[2] = color.b;
	pixels[3] = color.a;
}

vec4 fragment()
{
	vec2 uv = UV;
	vec2 uv_up = uv + vec2(0, TEXTURE_PIXEL_SIZE.y) * outline_width;
	vec2 uv_down = uv + vec2(0, -TEXTURE_PIXEL_SIZE.y) * outline_width;
	vec2 uv_left = uv + vec2(TEXTURE_PIXEL_SIZE.x, 0) * outline_width;
	vec2 uv_right = uv + vec2(-TEXTURE_PIXEL_SIZE.x, 0) * outline_width;

	vec4 color_up = texture(TEXTURE, uv_up);
	vec4 color_down = texture(TEXTURE, uv_down);
	vec4 color_left = texture(TEXTURE, uv_left);
	vec4 color_right = texture(TEXTURE, uv_right);

	vec4 outline = color_up + color_down + color_left + color_right;
	outline.r = outline_color.r;
	outline.g = outline_color.g;
	outline.b = outline_color.b;
	if (outline.a > 1)
		outline.a = 1;

	vec4 original_color = texture(TEXTURE, UV);
	vec4 COLOR = mix(outline, original_color, original_color.a);
	return COLOR;
}

int main()
{
	TEXTURE = stbi_load("witch.png", &WIDTH, &HEIGHT, &CHANNELS, 0);
	assert(TEXTURE);
	assert(WIDTH == 32);
	assert(HEIGHT == 288);
	assert(CHANNELS == 4);

	unsigned char *pixels = new unsigned char[WIDTH * HEIGHT * CHANNELS * 2];
	TEXTURE_PIXEL_SIZE.x = 1.0f / WIDTH;
	TEXTURE_PIXEL_SIZE.y = 1.0f / HEIGHT;
	size_t i = 0;
	for (float y = 0; y < 1; y += TEXTURE_PIXEL_SIZE.y)
	{
		for (float x = 0; x < 1; x += TEXTURE_PIXEL_SIZE.x)
		{
			UV = { x, y };
			putrgb(pixels + i, fragment());
			i += 4;
		}
	}

	stbi_write_png("out.png", WIDTH, HEIGHT, CHANNELS, pixels, WIDTH * CHANNELS);

	return 0;
}
