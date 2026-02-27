/*

https://en.wikipedia.org/wiki/Alpha_max_plus_beta_min_algorithm
http://dspguru.com/dsp/tricks/magnitude-estimator/
https://math.stackexchange.com/questions/1282435/alpha-max-plus-beta-min-algorithm-for-three-numbers

Approximation of the sqrt(a^2 + b^2)

*/

package main

import (
	"fmt"
	"math"
)

func main() {
	test(1e4, 1e1)
	test(2*math.Pi, 1e-3)
}

func test(N, S float64) {
	e := 0.0
	for y := -N; y <= N; y += S {
		for x := -N; x <= N; x += S {
			u := math.Hypot(x, y)
			v := alphabeta(x, y)
			if v != 0 {
				e = max(e, u/v)
			}
		}
	}
	fmt.Println(e)
}

func alphabeta(x, y float64) float64 {
	x = math.Abs(x)
	y = math.Abs(y)
	return ALPHA*max(x, y) + BETA*min(x, y)
}

var (
	ALPHA = (2 * math.Cos(math.Pi/8)) / (1 + math.Cos(math.Pi/8))
	BETA  = (2 * math.Sin(math.Pi/8)) / (1 + math.Cos(math.Pi/8))
)
