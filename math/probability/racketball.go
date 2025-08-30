/*
In raquetball, a player continues to serve as long as she is winning; a point
is scored only when a player is serving and wins the volley. The first player
to win 21 points wins the game. Assume that you serve first and have a
probability .6 of winning a volley when you serve and probability .5 when
your opponent serves. Estimate, by simulation, the probability that you will
win a game?

answer: running N simulations, get 75% wins for you, 25% for them
*/
package main

import (
	"fmt"
	"math/rand"
	"time"
)

const (
	N = 1e5
)

func main() {
	rand.Seed(time.Now().Unix())

	var w [2]int
	p := [2]float64{.6, .5}
	for i := 0; i < N; i++ {
		you := 0
		opp := 0
		trials := 0
		for t := 0; ; t ^= 1 {
			n := rand.Float64()
			if p[t] >= n {
				you++
			} else {
				opp++
			}
			trials++

			if you == 21 {
				w[0]++
				break
			} else if opp == 21 {
				w[1]++
				break
			}
		}
	}

	fmt.Println("you/opp:", w[0], w[1])
}
