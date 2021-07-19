/*
In Las Vegas, a roulette wheel has 38 slots numbered 0, 00, 1, 2, ..., 36. The
0 and 00 slots are green and half of the remaining 36 slots are red and half
are black. A croupier spins the wheel and throws in an ivory ball. If you bet
1 dollar on red, you win 1 dollar if the ball stops in a red slot and otherwise
you lose 1 dollar. Write a program to find the total winnings for a player who
makes 1000 bets on red.

Answer: for a thousand tries, on average you seem to lose on average 38% of the time
for -398 dollars lost
*/
package main

import (
	"fmt"
	"math/rand"
	"time"
)

const (
	N = 1e3
)

func main() {
	rand.Seed(time.Now().Unix())
	dollars := 0
	p := 12.0 / 38
	for i := 0; i < N; i++ {
		n := rand.Float64()
		if n <= p {
			dollars++
		} else {
			dollars--
		}
	}
	if dollars > 0 {
		fmt.Println(dollars, "dollars won")
	} else {
		fmt.Println(-dollars, "dollars lost")
	}
}
