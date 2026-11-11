// http://www.claysturner.com/dsp/digital_resonators.pdf
package main

import (
	"math"
)

func main() {
	test(360, deg2rad(0), deg2rad(1), gencos, 0x1)
	test(36530, .24453, 3546, gencos, 0x1)

	test(360, deg2rad(0), 53, gensincos, 0x2)
	test(360, deg2rad(0), deg2rad(1), gensincos, 0x2)
	test(360, 0.425, 53, gensincos, 0x2)
}

func test(n int, t0, dt float64, gen func([][2]float64, float64, float64), flags int) {
	const eps = 1e-8
	tab := make([][2]float64, n)
	gen(tab, t0, dt)

	t := t0
	for i := range tab {
		if flags&0x1 != 0 {
			assert(math.Abs(tab[i][0]-math.Cos(t)) < eps)
		}
		if flags&0x2 != 0 {
			assert(math.Abs(tab[i][1]-math.Sin(t)) < eps)
		}
		t += dt
	}
}

func deg2rad(x float64) float64 {
	return x * math.Pi / 180
}

// tab = table to fill where each index represent an increase in step size
// t0 = starting angle
// dt = angle step size
func gencos(tab [][2]float64, t0, dt float64) {
	// cos(p+q) = 2*cos(p)*cos(q) - cos(p-q)
	// p = t0
	// q = dt
	a := math.Cos(t0)
	b := math.Cos(t0 - dt)
	k := 2 * math.Cos(dt)
	for i := range tab {
		tab[i][0] = a
		a, b = k*a-b, a
	}
}

// tab = table to fill where each index represent an increase in step size
// t0 = starting angle
// dt = angle step size
func gensincos(tab [][2]float64, t0, dt float64) {
	// cos(p+q) = cos(p)*cos(q) - sin(p)*sin(q)
	// sin(p+q) = sin(p)*cos(q) + cos(p)*sin(q)
	// p = t0
	// q = dt
	b, a := math.Sincos(t0)
	k2, k1 := math.Sincos(dt)
	for i := range tab {
		tab[i][0] = a
		tab[i][1] = b
		a, b = a*k1-b*k2, b*k1+a*k2
	}
}

func assert(x bool) {
	if !x {
		panic("assertion failed")
	}
}
