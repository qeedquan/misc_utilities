// https://www.kalmanfilter.net/kalman1d.html
package main

import (
	"fmt"
	"math"
	"math/rand"
	"time"
)

func main() {
	rand.Seed(time.Now().UnixNano())

	m0 := []float64{48.54, 47.11, 55.01, 55.15, 49.89, 40.85, 46.72, 50.05, 51.27, 49.95}
	m1 := []float64{49.95, 49.967, 50.1, 50.106, 49.992, 49.819, 49.933, 50.007, 50.023, 49.99}
	v0 := 1340.0
	v1 := 100000.0

	sim(len(m0), 50, 60, 5, 15, mklutf(m0))
	sim(len(m1), 50, 10, 0.1, 100, mklutf(m1))
	sim(50, v0, 60, 10, 100, mkrandf(v0, 100))
	sim(1000, v1, -452, 10, 100, mkrandf(v1, 100))
}

func mklutf(tab []float64) func() float64 {
	i := 0
	return func() float64 {
		if len(tab) == 0 {
			return 0
		}
		v := tab[i]
		i = (i + 1) % len(tab)
		return v
	}
}

func mkrandf(value, deviation float64) func() float64 {
	return func() float64 {
		scale := (2 * rand.Float64()) - 1
		return value + scale*deviation
	}
}

func sim(iters int, value, guess, measure_stddev, estimate_stddev float64, measure func() float64) {
	fmt.Println("Estimating", value)
	x := guess
	p := estimate_stddev * estimate_stddev
	r := measure_stddev * measure_stddev
	for i := 0; i < iters; i++ {
		K := p / (p + r)
		xn := x + K*(measure()-x)
		pn := (1 - K) * p

		fmt.Printf("%d x %.3f p %.3f xn %.3f pn %.3f K %.3f err %.6f\n",
			i+1, x, p, xn, pn, K, math.Abs(value-x))

		x = xn
		p = pn
	}
	fmt.Println()
}
