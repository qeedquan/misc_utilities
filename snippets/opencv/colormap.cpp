#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;

void apply_colormap(Mat img, const char *filename, ColormapTypes type)
{
	Mat img_color;
	applyColorMap(img, img_color, type);
	imwrite(filename, img_color);
}

int main(int argc, const char *argv[])
{
	if (argc < 2)
	{
		cerr << "usage: <file>" << endl;
		return 1;
	}

	Mat img = imread(argv[1]);
	if (img.empty())
	{
		cerr << "Could not load sample image" << endl;
		return 1;
	}

	apply_colormap(img, "colormap_autumn.png", COLORMAP_AUTUMN);
	apply_colormap(img, "colormap_bone.png", COLORMAP_BONE);
	apply_colormap(img, "colormap_jet.png", COLORMAP_JET);
	apply_colormap(img, "colormap_winter.png", COLORMAP_WINTER);
	apply_colormap(img, "colormap_rainbow.png", COLORMAP_RAINBOW);
	apply_colormap(img, "colormap_ocean.png", COLORMAP_OCEAN);
	apply_colormap(img, "colormap_summer.png", COLORMAP_SUMMER);
	apply_colormap(img, "colormap_spring.png", COLORMAP_SPRING);
	apply_colormap(img, "colormap_cool.png", COLORMAP_COOL);
	apply_colormap(img, "colormap_hsv.png", COLORMAP_HSV);
	apply_colormap(img, "colormap_pink.png", COLORMAP_PINK);
	apply_colormap(img, "colormap_hot.png", COLORMAP_HOT);
	apply_colormap(img, "colormap_parula.png", COLORMAP_PARULA);
	apply_colormap(img, "colormap_magma.png", COLORMAP_MAGMA);
	apply_colormap(img, "colormap_inferno.png", COLORMAP_INFERNO);
	apply_colormap(img, "colormap_plasma.png", COLORMAP_PLASMA);
	apply_colormap(img, "colormap_viridis.png", COLORMAP_VIRIDIS);
	apply_colormap(img, "colormap_cividis.png", COLORMAP_CIVIDIS);
	apply_colormap(img, "colormap_twilight.png", COLORMAP_TWILIGHT);
	apply_colormap(img, "colormap_twilight_shifted.png", COLORMAP_TWILIGHT_SHIFTED);
	apply_colormap(img, "colormap_turbo.png", COLORMAP_TURBO);
	apply_colormap(img, "colormap_deepgreen.png", COLORMAP_DEEPGREEN);

	return 0;
}
