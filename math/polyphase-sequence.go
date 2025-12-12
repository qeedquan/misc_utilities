// https://en.wikipedia.org/wiki/Polyphase_sequence
package main

import (
	"fmt"
	"math"
	"math/cmplx"
)

func main() {
	q := 0.45
	for i := 0; i <= 10; i++ {
		fmt.Printf("%.3v %.3v\n", polyphase(q, i), polyphase(-q, i))
	}
}

// a sequence whose terms are complex roots of unity
func polyphase(q float64, x int) complex128 {
	return cmplx.Exp(complex(0, 2*math.Pi/q*float64(x)))
}
