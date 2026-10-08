/*

https://en.wikipedia.org/wiki/Smith_chart
https://en.wikipedia.org/wiki/M%C3%B6bius_transformation

*/

package main

import (
	"flag"
	"fmt"
	"image"
	"image/color"
	"image/draw"
	"image/png"
	"log"
	"os"
)

var opt struct {
	width  int
	height int
	slices int
	spsize float64
	spstep float64
}

func main() {
	parseflags()
	m := image.NewRGBA(image.Rect(0, 0, opt.width, opt.height))
	drawgrid(m, opt.slices, opt.spsize, opt.spstep)
	f, err := os.Create(flag.Arg(0))
	ck(err)
	ck(png.Encode(f, m))
	ck(f.Close())
}

func parseflags() {
	flag.IntVar(&opt.width, "w", 2048, "image width")
	flag.IntVar(&opt.height, "h", 2048, "image height")
	flag.IntVar(&opt.slices, "n", 20, "grid slice size")
	flag.Float64Var(&opt.spsize, "s", 0.4, "supersampling size")
	flag.Float64Var(&opt.spstep, "t", 0.02, "supersampling step")

	flag.Parse()
	if flag.NArg() != 1 {
		usage()
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "[options] smith.png")
	flag.PrintDefaults()
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

/*

A smith chart can be generated using a mobius transformation of (z-1)/(z+1) on the complex plane
They are usually generated on a log grid rather than linear but we generate a linear grid here

*/

func drawgrid(m *image.RGBA, n int, s, ds float64) {
	r := m.Bounds()
	w := r.Dx()
	h := r.Dy()

	draw.Draw(m, r, image.NewUniform(color.White), image.ZP, draw.Src)
	for y := 0; y <= h; y++ {
		for x := 0; x <= w; x++ {
			if x%n == 0 || y%n == 0 {
				for i := -s; i <= s; i += ds {
					for j := -s; j <= s; j += ds {
						zx := remap(float64(x)+i, 0, float64(w), -1, 1)
						zy := remap(float64(y)+j, 0, float64(h), -1, 1)
						z := complex(zx, zy)
						z = (z - 1) / (z + 1)

						px := remap(real(z), -4, 2, 0, float64(w))
						py := remap(imag(z), -4, 2, 0, float64(h))

						px += float64(w) / 2

						m.SetRGBA(int(px+0.5), int(py+0.5), color.RGBA{A: 255})
					}
				}
			}
		}
	}
}

func lerp(t, a, b float64) float64        { return a + t*(b-a) }
func unlerp(t, a, b float64) float64      { return (t - a) / (b - a) }
func remap(x, a, b, c, d float64) float64 { return lerp(unlerp(x, a, b), c, d) }
