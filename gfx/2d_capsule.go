package main

import (
	"flag"
	"image"
	"image/color"
	"image/png"
	"math"
	"os"
)

func main() {
	var a, r, n float64

	flag.Float64Var(&a, "a", 4, "specify a")
	flag.Float64Var(&r, "r", 4, "specify r")
	flag.Float64Var(&n, "n", 5, "specify n")
	flag.Parse()

	m := capsule(800, 800, a, r, n)
	png.Encode(os.Stdout, m)
}

/*

https://math.stackexchange.com/questions/2678480/formula-for-a-stadium-shape-2d-capsule

@Robert Howard
(x - a/2 - sqrt(r^2 - y^2)) * (x + a/2 + sqrt(r^2 - y^2)) * (y - r) * (y + r) = 10^-n

a and r here are exactly the same parameters as in the diagram you included.
n is a "quality factor" of sorts; the larger it gets, the sharper the "corners" of the stadium get (i.e. the points (±a2,±a)).
When n=5, the graph is nearly indistinguishable from when n=∞ (i.e. when the right side of the equation is 0).

I didn't really derive that formula;
I just played around in Desmos until I found something that worked.
Each term in parentheses, when set equal to 0, produces one of the four parts of the figure -- either a circular arc or a horizontal line.
Multiplying the terms together trims the horizontal lines at the points where they intersect the arcs for reasons I might have understood 4 years ago but don't today. For more reasons I don't remember, when the right-hand side is 0, the line segments disappear.
But making it close to zero (i.e. 10e-5) achieves the desired affect.

*/

func capsule(w, h int, a, r, n float64) *image.RGBA {
	m := image.NewRGBA(image.Rect(0, 0, w, h))
	for y := range h {
		for x := range w {
			X := remap(float64(x), 0, float64(w), -a*2, a*2)
			Y := remap(float64(y), 0, float64(h), -a*2, a*2)
			Z := r*r - Y*Y
			if Z < 0 {
				continue
			}
			k0 := X - a/2 - math.Sqrt(Z)
			k1 := X + a/2 + math.Sqrt(Z)
			k2 := Y - r
			k3 := Y + r
			k4 := math.Pow(10, -n)
			if k0*k1*k2*k3-k4 >= 0 {
				m.SetRGBA(x, y, color.RGBA{30, 40, 50, 255})
			}
		}
	}
	return m
}

func lerp(t, a, b float64) float64 {
	return a + t*(b-a)
}

func unlerp(t, a, b float64) float64 {
	return (t - a) / (b - a)
}

func remap(x, a, b, c, d float64) float64 {
	return lerp(unlerp(x, a, b), c, d)
}
