// Tosses a coin and times and records
// whether or not the proportion of heads is within .1 of .5 (i.e., between .4
// and .6). Have your program repeat this experiment 100 times. About how
// large must n be so that approximately 95 out of 100 times the proportion of
// heads is between .4 and .6?
// Answer: about 1-sqrt(n) >= 95 for a uniform random dice
// we increment the time when it hits heads, minus one otherwise,
// we do this so it can "remember", if we reset every 100 times,
// it is as if it forgets what it obtained
package main

import (
	"fmt"
	"math"
	"math/rand"
	"time"
)

func main() {
	rand.Seed(time.Now().Unix())

	heads := 0
	for t := 0; t < 100; t++ {
		n := rand.Float64()
		if .4 < n && n < .6 {
			heads++
		}
	}
	fmt.Println("100 times:", heads)

	h := 0
	times := 0
	heads = 0
	for {
		n := rand.Float64()
		if .4 < n && n < 6 {
			heads++
			h++
		} else {
			h--
		}
		if h >= 95 {
			break
		}
		times++
	}
	fmt.Printf("%v times: %v %v\n", times, heads, float64(heads)/float64(times))
	fmt.Println("error rate:", 1/math.Sqrt(float64(times)))
}
