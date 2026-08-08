// Ported from https://github.com/rlauzon54/PicoCalcBasic/blob/main/bench.bas
// Ahl's Simple Benchmark
package main

import (
	"fmt"
	"math"
	"math/rand"
	"time"
)

func main() {
	s := 0.0
	r := 0.0
	t0 := time.Now()
	for n := 1; n <= 100; n++ {
		a := n
		for _ = range 10 {
			a = sqr(a)
			r = r + rand.Float64()
		}

		for _ = range 10 {
			a = a * a
			r = r + rand.Float64()
		}
		s += float64(a)
	}
	fmt.Println("Time:", time.Since(t0))
	fmt.Println("Accuracy:", math.Abs(1010-s/5))
	fmt.Println("Random:", math.Abs(1000-r))
}

func sqr(x int) int {
	return x * x
}
