// http://planning.cs.uiuc.edu/node210.html

package main

import (
	"fmt"
	"math"
	"math/bits"

	"golang.org/x/image/math/f64"
)

func main() {
	N := 1024
	for i := 0; i < N; i++ {
		fmt.Println(hammersley32(uint32(i), float64(N)), hammersley64(uint64(i), float64(N)))
	}
}

// the hammersley sequence generate low-discrepancy points for sampling
// these are specific values used for fast calculations by using base-2
// i represents the ith sample point, for each sample point,
// reverse the binary representation and divide that by 2^n
// generate points in [0,1]
func hammersley32(i uint32, n float64) f64.Vec2 {
	bits := bits.Reverse32(i)
	return f64.Vec2{float64(i) / n, float64(bits) / math.Exp2(32)}
}

func hammersley64(i uint64, n float64) f64.Vec2 {
	bits := bits.Reverse64(i)
	return f64.Vec2{float64(i) / n, float64(bits) / math.Exp2(64)}
}
