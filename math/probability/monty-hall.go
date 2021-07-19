// https://en.wikipedia.org/wiki/Monty_Hall_problem
package main

import (
	"fmt"
	"math/rand"
	"time"
)

func main() {
	rand.Seed(time.Now().UnixNano())

	const N = 1e6
	fmt.Printf("%.3f\n", montyhall(N, true, true))
	fmt.Printf("%.3f\n", montyhall(N, true, false))

	fmt.Printf("%.3f\n", montyhall(N, false, true))
	fmt.Printf("%.3f\n", montyhall(N, false, false))
}

func montyhall(N int, f bool, s bool) float64 {
	p := []int{1, 0, 0}
	w := 0
	for n := 0; n < N; n++ {
		rand.Shuffle(len(p), func(i, j int) {
			p[i], p[j] = p[j], p[i]
		})

		x := rand.Intn(len(p))
		y := (x + 1) % len(p)
		z := (x + 2) % len(p)

		// this is where the paradox lies
		// if we allow swaps, then the probability
		// will increase to 2/3, if we do not,
		// then probability of winning is still 1/3
		// what this swap is saying is that the host
		// always know where the car is, and if the original guess
		// by the player was not correct, the host will therefore
		// pick the one that always NOT a car
		// So he will "swap" the choice if his choice was a car
		// for the one that is not, and this changes the probability
		if p[y] != 0 && s {
			p[y], p[z] = p[z], p[y]
		}

		if f {
			x = z
		}
		w += p[x]
	}
	return float64(w) / float64(N)
}
