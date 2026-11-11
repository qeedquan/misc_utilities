// For every 100 coin flips, record the number of heads encountered
// and subtract .5 from it, does it go to 0 as the number of coin flips
// go to infinity? Answer: yes
// Since it is a uniform and fair dice, the probability as n goes to
// infinity, it should be .5
package main

import (
	"fmt"
	"math/rand"
	"time"
)

func main() {
	rand.Seed(time.Now().Unix())

	times := 0
	heads := 0
	for {
		n := rand.Float64()
		if n <= .5 {
			heads++
		}
		if times++; times%100 == 0 {
			fmt.Println(times, float64(heads)/float64(times))
		}
	}
}
