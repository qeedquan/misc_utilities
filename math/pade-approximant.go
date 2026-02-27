/*

https://en.wikipedia.org/wiki/Pad%C3%A9_approximant
https://mathworld.wolfram.com/PadeApproximant.html

*/

package main

import (
	"fmt"
	"math"
)

func main() {
	p_sin := [2][]float64{
		{0, 1, 0, -2363.0 / 18183.0, 0, 12671.0 / 4363920.0, 0.0},
		{1, 0, 445.0 / 12122.0, 0, 601.0 / 872784.0, 0, 121.0 / 16662240.0},
	}

	p_exp := [2][]float64{
		{1, 1.0 / 2.0, 1.0 / 9.0, 1.0 / 72.0, 1.0 / 1008.0, 1.0 / 30240.0},
		{1, -1.0 / 2.0, 1.0 / 9.0, -1.0 / 72.0, 1.0 / 1008.0, -1.0 / 30240.0},
	}

	p_erf := [2][]float64{
		{0, 49140, 0, 3570, 0, 739},
		{3276, 0, 1330, 0, 165, 0},
	}
	scale(p_erf, 2/15.0, math.Sqrt(math.Pi))

	cmp(0, 2*math.Pi, 1e-4, math.Sin, p_sin)
	cmp(-5, 5, 1e-4, math.Exp, p_exp)
	cmp(-2, 2, 1e-4, math.Erf, p_erf)
}

func scale(R [2][]float64, s0, s1 float64) {
	for i := range R {
		R[0][i] *= s0
		R[1][i] *= s1
	}
}

func pade(x float64, R [2][]float64) float64 {
	p := 0.0
	q := 0.0
	u := 1.0
	for i := range R[0] {
		p += R[0][i] * u
		q += R[1][i] * u
		u *= x
	}
	return p / q
}

func cmp(x0, x1, dx float64, f func(float64) float64, R [2][]float64) {
	m := 0.0
	xm := x0
	for x := x0; x <= x1; x += dx {
		a := f(x)
		b := pade(x, R)
		e := math.Abs(a - b)
		if m < e {
			m = e
			xm = x
		}
	}
	fmt.Printf("range [%.3f, %.3f] step %.3f maxerrpoint %.3f maxerr %.3f\n", x0, x1, dx, xm, m)
}
