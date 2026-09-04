// https://en.wikipedia.org/wiki/Golden-section_search
// finds extremum in function in interval, for strictly unimodal
// with extremum, it will find it, if multiple extremum, it will find
// one of them
package main

import (
	"fmt"
	"math"
)

func main() {
	f1 := func(x float64) float64 { return (x - 2) * (x - 2) }
	fmt.Println(gss(f1, 1, 5, 1e-8))

	f2 := func(x float64) float64 { return 5 }
	fmt.Println(gss(f2, -1e3, 1e3, 1e-8))

	f3 := func(x float64) float64 {
		if x == 42 {
			return -200
		}
		return 0
	}
	fmt.Println(gss(f3, 0, 42, 1e-8))

	f4 := func(x float64) float64 { return math.Abs(x-2) + (x-1)*(x-1) }
	fmt.Println(gss(f4, 0, 1e2, 1e-8))

	f5 := func(x float64) float64 { return math.Pow(x, 5) }
	fmt.Println(gss(f5, -1e2, 1e2, 1e-8))

	f6 := func(x float64) float64 { return 2 * math.Cos(1000*x) }
	fmt.Println(gss(f6, 0, math.Pi/4, 1e-8))
	fmt.Println(gss(f6, math.Pi/4, math.Pi/2, 1e-8))
	fmt.Println(gss(f6, math.Pi/2, math.Pi, 1e-8))
}

func gss(f func(float64) float64, a, b, tol float64) (float64, float64) {
	var (
		invphi  = (math.Sqrt(5) - 1) / 2
		invphi2 = (3 - math.Sqrt(5)) / 2
	)

	a, b = math.Min(a, b), math.Max(a, b)
	h := b - a
	if h <= tol {
		return a, b
	}

	n := int(math.Ceil(math.Log(tol/h) / math.Log(invphi)))
	c := a + invphi2*h
	d := a + invphi*h
	yc := f(c)
	yd := f(d)

	for k := 0; k < n; k++ {
		if yc < yd {
			b = d
			d = c
			yd = yc
			h = invphi * h
			c = a + invphi2*h
			yc = f(c)
		} else {
			a = c
			c = d
			yc = yd
			h = invphi * h
			d = a + invphi*h
			yd = f(d)
		}
	}

	if yc < yd {
		return a, d
	} else {
		return c, b
	}
}
