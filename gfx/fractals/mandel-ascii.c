// de-obfuscated version of https://mrl.nyu.edu/~perlin/
#include <stdio.h>

void
mandelbrot(int w, int h)
{
	static const char sym[] = " .:-;!/>)|&IH%*#";

	float i, j, r, x, y;
	int k;

	for (y = -h / 2; y <= h / 2; y++) {
		puts("");
		for (x = 0; x < w; x++) {
			i = j = k = r = 0;
			do {
				j = r * r - i * i - 2 + x / 25;
				i = 2 * r * i + y / 10;
				r = j;
			} while (j * j + i * i < 11 && k++ < 111);
			putchar(sym[k & 15]);
		}
	}
	puts("");
}

int
main(void)
{
	mandelbrot(84, 25);
	mandelbrot(100, 100);
	mandelbrot(256, 25);
	return 0;
}
