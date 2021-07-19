// https://en.wikipedia.org/wiki/Damping_ratio
package main

import (
	"fmt"
	"math"
)

func main() {
	for zeta := 0.0; zeta <= 1.0; zeta += 1e-3 {
		fmt.Println(percentage_overshoot(zeta))
	}

	for po := 0.0; po <= 100; po++ {
		fmt.Println(damping_ratio(po))
	}

	fmt.Println(zeta2qfactor(qfactor2zeta(255634.536)))
}

// the percentage overshoot is an exponential decay
// zeta [0, 1] -> [100%, 0%]
func percentage_overshoot(zeta float64) float64 {
	return 100 * math.Exp(-(zeta*math.Pi)/(math.Sqrt(1-zeta*zeta)))
}

// damping ratio
// determines how the systems tend to oscillate
// zeta = 0 undamped (oscillates indefinitely)
// 0 <= zeta < 1 underdamped (overshoot and oscillate but slowly goes back to rest)
// zeta = 1 critically damped (returns to rest in the shortest time with no overshoot)
// zeta > 1 overdamped (returns to rest without overshooting)
func damping_ratio(po float64) float64 {
	l := math.Log(po / 100)
	return -l / math.Sqrt(math.Pi*math.Pi+l*l)
}

func zeta2qfactor(zeta float64) float64 {
	return 1 / (2 * zeta)
}

func qfactor2zeta(q float64) float64 {
	return 1 / (2 * q)
}
