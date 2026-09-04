/*
Write a program to simulate a random walk in one dimension starting
at 0. Have your program print out the lengths of the times between
returns to the starting point (returns to 0). See if you can guess from
this simulation the answer to the following question: Will the walker
always return to his starting point eventually or might he drift away
forever?
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
	for i := 0; i < N; i++ {
		p := 0
		steps := 0
		for {
			n := rand.Float64()
			if n >= .5 {
				p++
			} else {
				p--
			}
			steps++

			if p == 0 {
				break
			}
		}
		fmt.Println(i, steps)
	}
}
