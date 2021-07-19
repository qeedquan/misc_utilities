// https://en.wikipedia.org/wiki/Superformula
// https://gpfault.net/posts/superformula.txt.html
package main

import (
	"image"
	"image/color"
	"image/draw"
	"image/png"
	"log"
	"math"
	"os"

	"github.com/qeedquan/go-media/math/f64"
)

func main() {
	P := [][4]float64{
		{3, 4.5, 10, 10},
		{4, 12, 15, 15},
		{7, 10, 6, 6},
		{5, 4, 4, 4},
		{5, 2, 7, 7},
		{5, 2, 13, 13},
		{4, 1, 1, 1},
		{4, 1, 7, 8},
		{6, 1, 7, 8},
		{2, 2, 2, 2},
		{1, 0.5, 0.5, 0.5},
		{2, 0.5, 0.5, 0.5},
		{3, 0.5, 0.5, 0.5},
		{5, 1, 1, 1},
		{2, 1, 1, 1},
		{7, 3, 4, 17},
		{2, 1, 4, 8},
		{6, 1, 4, 8},
		{7, 2, 8, 4},
		{4, 0.5, 0.5, 4},
		{8, 0.5, 0.5, 8},
		{16, 0.5, 0.5, 16},
		{3, 30, 15, 15},
		{4, 30, 15, 15},
		{16, 2, 0.5, 16},
	}
	r := image.Rect(0, 0, 1024, 1024)
	m := image.NewRGBA(r)
	draw.Draw(m, m.Bounds(), image.NewUniform(color.RGBA{128, 128, 128, 255}), image.ZP, draw.Src)

	for i, P := range P {
		N := 4
		s := r.Dx() / 2 / N
		ox := (i%N)*s + s/2
		oy := (i/N)*s + s/2

		// parametric form, increment a small angle, calculate r
		// then convert to rectangular coordinate to set the pixel, need a small step
		// so we don't miss points
		for t := 0.0; t <= 2*math.Pi; t += 1e-4 {
			r := 8 * super(t, 1, 1, P[0], P[0], P[1], P[2], P[3])
			x := r*math.Cos(t) + float64(ox)
			y := r*math.Sin(t) + float64(oy)
			m.Set(int(x), int(y), color.RGBA{224, 224, 255, 255})
		}

		// implicit distance form, we iterate through our draw viewport
		// determine if the distance is within the shape boundary and draw it
		D := float64(s)
		// zoom factor
		Z := 3.0
		for y := -D; y <= D; y++ {
			for x := -D; x <= D; x++ {
				// all parameters used seem to have domain [-5,5] (determined empirically)
				px := f64.LinearRemap(x, -D, D, -5*Z, 5*Z)
				py := f64.LinearRemap(y, -D, D, -5*Z, 5*Z)
				ds := imsuper(px, py, 1, 1, P[0], P[0], P[1], P[2], P[3])
				if ds <= 1 {
					m.Set(int(x+float64(ox)+float64(r.Dx())/2), int(y+float64(oy)), color.RGBA{224, 224, 255, 255})
				}
			}
		}
	}
	f, err := os.Create("superformula.png")
	ck(err)
	ck(png.Encode(f, m))
	ck(f.Close())
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func super(phi, a, b, m1, m2, n1, n2, n3 float64) float64 {
	u := math.Abs(math.Cos(m1*phi/4) / a)
	v := math.Abs(math.Sin(m2*phi/4) / b)

	u = math.Pow(u, n2)
	v = math.Pow(v, n3)

	return math.Pow(u+v, -1/n1)
}

func imsuper(x, y, a, b, m1, m2, n1, n2, n3 float64) float64 {
	r := math.Hypot(x, y)
	phi := math.Atan2(y, x)
	v := super(phi, a, b, m1, m2, n1, n2, n3)
	return r - v
}
