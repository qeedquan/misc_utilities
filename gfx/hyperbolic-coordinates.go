// https://en.wikipedia.org/wiki/Hyperbolic_coordinates
package main

import (
	"flag"
	"image"
	"image/color"
	"image/draw"
	"image/png"
	"log"
	"math"
	"os"
)

var (
	width   = flag.Int("w", 800, "width")
	height  = flag.Int("h", 800, "height")
	outfile = flag.String("o", "hp.png", "output file")
)

func main() {
	flag.Parse()

	m := image.NewRGBA(image.Rect(0, 0, *width, *height))
	plot(m)

	f, err := os.Create(*outfile)
	ck(err)

	png.Encode(f, m)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

// cartesian to hyperbolic
func chp(x, y float64) (u, v float64) {
	if y == 0 {
		return
	}
	u = math.Log(math.Sqrt(x / y))
	v = math.Sqrt(x * y)
	return
}

// hyperbolic to cartesian
func hpc(u, v float64) (x, y float64) {
	x = v * math.Exp(u)
	y = v * math.Exp(-u)
	return
}

// draws a quadrant 1 image described in wiki
func plot(m *image.RGBA) {
	r := m.Bounds()
	x1 := float64(r.Max.X)
	y1 := float64(r.Max.Y)
	draw.Draw(m, r, image.NewUniform(color.RGBA{240, 240, 240, 255}), image.ZP, draw.Src)

	u0 := -2.0
	u1 := 2.0
	v0 := 0.0
	_, v1 := chp(x1, y1)
	du := 1e-1
	dv := 1e-1
	for u := u0; u <= u1; u += du {
		for v := v0; v <= v1; v += dv {
			x, y := hpc(u, v)
			m.Set(int(x+0.5), int(y1-y+0.5), color.RGBA{0, 0, 255, 255})
		}
	}

	u0 = -2.0
	u1 = 2.0
	v0 = 1.0
	v1 = y1
	du = 1e-4
	dv = 20
	for u := u0; u <= u1; u += du {
		for v := v0; v <= v1; v += dv {
			x, y := hpc(u, v)
			m.Set(int(x+0.5), int(y1-y+0.5), color.RGBA{255, 0, 0, 255})
		}
	}
}
