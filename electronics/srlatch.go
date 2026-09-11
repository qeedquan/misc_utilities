package main

import "fmt"

func main() {
	truth()
}

// https://www.allaboutcircuits.com/textbook/digital/chpt-10/s-r-latch/

// print truth table for SR latch device

// SR latch, this is a bistable device
// valid states:
// S R = 0 0 -> latched Q and ^Q (Q retains its previous value)
// S R = 0 1 -> 0 1
// S R = 1 0 -> 1 0
// S R = 1 1 -> 0 0

// the illegal state is where Q and ^Q is both 0, so
// don't set S and R to be both 1

// notice how this device does not depend on the value of Q
// and only S and R for it to be latched or not.
func truth() {
	fmt.Printf("S R Q QB RQ SQB\n")
	for i := uint(0); i < 4; i++ {
		S := i & 1
		R := (i >> 1) & 1

		for j := uint(0); j < 2; j++ {
			Q := j
			QB := ^Q & 1

			RQ := ^(R | ^(S | Q)) & 1
			SQB := ^(S | ^(R | QB)) & 1

			fmt.Printf("%d %d %d %-2d %-2d %-2d\n", S, R, Q, QB, RQ, SQB)
		}
	}
}
