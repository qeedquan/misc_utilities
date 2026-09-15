// https://mathinsight.org/spherical_coordinates
package main

import (
	"flag"
	"fmt"
	"image"
	"image/color"
	"image/draw"
	"image/png"
	"log"
	"math"
	"os"
)

var (
	width  = flag.Int("width", 1024, "width")
	height = flag.Int("height", 1024, "height")
	radius = flag.Float64("radius", 1, "radius")
	step   = flag.Float64("step", 1e-3, "step")
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}
	m := image.NewRGBA(image.Rect(0, 0, *width, *height))
	spherecontour(m, *radius, *step)
	f, err := os.Create(flag.Arg(0))
	ck(err)
	ck(png.Encode(f, m))
	ck(f.Close())
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] sphere_contour.png")
	flag.PrintDefaults()
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func spherecontour(m *image.RGBA, r, s float64) {
	b := m.Bounds()
	w := float64(b.Dx())
	h := float64(b.Dy())

	draw.Draw(m, b, image.NewUniform(color.White), image.ZP, draw.Src)
	for p := 0.0; p <= math.Pi; p += s {
		sp, cp := math.Sincos(p)
		for t := 0.0; t <= 2*math.Pi; t += s {
			st, ct := math.Sincos(t)

			rx := r * sp * ct
			ry := r * sp * st
			rz := r * cp

			x := rx / (1 - rz)
			y := ry / (1 - rz)
			x += w / 2
			y += h / 2

			m.SetRGBA(int(x), int(y), color.RGBA{0, 0, 0, 255})
		}
	}
}
