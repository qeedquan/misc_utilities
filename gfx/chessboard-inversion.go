/*

https://mathworld.wolfram.com/Inversion.html

*/

package main

import (
	"flag"
	"image"
	"image/color"
	"image/draw"
	"image/png"
	"log"
	"os"
)

var (
	width  = flag.Int("width", 1024, "image width")
	height = flag.Int("height", 1024, "image height")
	slices = flag.Int("slices", 32, "number of slices")
	radius = flag.Float64("radius", 128, "radius")
	spsize = flag.Float64("spsize", 0.5, "supersampling size")
	spstep = flag.Float64("spstep", 0.02, "supersampling step size")
)

func main() {
	flag.Parse()
	m := image.NewRGBA(image.Rect(0, 0, *width, *height))
	render(m, *slices, *radius, *spsize, *spstep)
	f, err := os.Create("chessboard_inversion.png")
	ck(err)
	ck(png.Encode(f, m))
	ck(f.Close())
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func render(m *image.RGBA, s int, k float64, sp, dsp float64) {
	r := m.Bounds()
	draw.Draw(m, r, image.NewUniform(color.White), image.ZP, draw.Src)

	x0 := float64(r.Max.X+r.Min.X) / 2
	y0 := float64(r.Max.Y+r.Min.Y) / 2
	for y := r.Min.Y; y < r.Max.Y; y++ {
		for x := r.Min.X; x < r.Max.X; x++ {
			col := color.RGBA{0, 0, 0, 255}
			xs := x / s
			ys := y / s
			if (xs^ys)&0x1 != 0 {
				col = color.RGBA{255, 255, 255, 255}
			}

			// we need to supersample to not miss points
			// faster way would be to interpolate
			for i := -sp; i <= sp; i += dsp {
				for j := -sp; j <= sp; j += dsp {
					xi, yi := inversion(float64(x)+j, float64(y)+i, x0, y0, k)
					m.SetRGBA(int(xi), int(yi), col)
				}
			}
		}
	}
}

// this function is an involution
func inversion(x, y, x0, y0, k float64) (xi, yi float64) {
	l := (x-x0)*(x-x0) + (y-y0)*(y-y0)
	xi = x0 + k*k*(x-x0)/l
	yi = y0 + k*k*(y-y0)/l
	return
}
