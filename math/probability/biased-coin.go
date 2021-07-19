// A biased coin is a coin that returns heads
// at probability P and (1-P) for tails
// to unbias a bias coin, toss it twice for the following outcomes:
// HH - PP
// HT - P(1-P)
// TH - (1-P)P
// TT - (1-P)(1-P)
// We can see that HT and TH have the same probability as each other
// so we can treat HT pair as a H and TH as a T, and discard the other result
// This can loop forever if P is 0 or 1, so need to have a bail condition
package main

import (
	"flag"
	"fmt"
	"math/rand"
	"strconv"
	"time"
)

func main() {
	rand.Seed(time.Now().UnixNano())
	flag.Parse()

	B := 0.1
	if flag.NArg() >= 1 {
		B, _ = strconv.ParseFloat(flag.Arg(0), 64)
	}

	const N = 1e6
	h := 0
	uh := 0
	for i := 0; i < N; i++ {
		r := toss(B)
		s := unbias(B)
		if r != 0 {
			h++
		}
		if s != 0 {
			uh++
		}
		if s == -1 {
			uh = h
		}
	}
	fmt.Println(float64(h) / N)
	fmt.Println(float64(uh) / N)
}

func toss(hp float64) int {
	r := rand.Float64()
	if r <= hp {
		return 1
	}
	return 0
}

func unbias(hp float64) int {
	var r [2]int
	for n := 0; n < 1e2; n++ {
		for i := 0; i < 2; i++ {
			r[i] = toss(hp)
		}
		if r[0] == 1 && r[1] == 0 {
			return 1
		}
		if r[0] == 0 && r[1] == 1 {
			return 0
		}
	}
	return -1
}
