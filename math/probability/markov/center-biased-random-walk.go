/*

https://en.wikipedia.org/wiki/Examples_of_Markov_chains

*/

package main

import (
	"fmt"
	"math"
	"math/rand"
	"os"
	"text/tabwriter"
	"time"
)

func main() {
	rand.Seed(time.Now().UnixNano())
	table(-5, 5, 1, 5)
	trial(1e5, -10, 10, 1)
	trial(1e5, -10, 10, 5)
	trial(1e5, -10, 10, 10)
	trial(1e5, -10, 10, 20)
}

func trial(n int, x1, x2, c float64) {
	for x := x1; x <= x2; x++ {
		sim(n, x, c)
	}
	fmt.Println()
}

/*

Center biased random walk in one dimension.
It can either go left or right but will drift torwards the center (at location 0 in 1D) based on the definition below

left   = 0.5 + 0.5*(x / (c + |x|)) where c > 0
right  = 1 - left

c = 1 for a center biased random walk, if it is bigger, it will start drifting away from the center

*/

func table(x1, x2, c1, c2 float64) {
	w := tabwriter.NewWriter(os.Stdout, 1, 4, 1, ' ', 0)
	defer w.Flush()

	fmt.Fprintf(w, "\t")
	for c := c1; c <= c2; c++ {
		fmt.Fprintf(w, "%v\t", c)
	}
	fmt.Fprintf(w, "\n")

	for x := x1; x <= x2; x++ {
		fmt.Fprintf(w, "% v\t", x)
		for c := c1; c <= c2; c++ {
			l, _ := prob(x, c)
			fmt.Fprintf(w, "%.6f\t", l)
		}
		fmt.Fprintf(w, "\n")
	}
	fmt.Fprintf(w, "\n")
}

func sim(n int, x, c float64) {
	var h [2]int
	for i := 0; i < n; i++ {
		l := 0.5 + 0.5*(x/(c+math.Abs(x)))
		if rand.Float64() <= l {
			x, h[0] = x-1, h[0]+1
		} else {
			x, h[1] = x+1, h[1]+1
		}
	}
	fmt.Printf("Simulation (run=%d) ", n)
	fmt.Printf("constant=%f position=%f left=%f right=%f\n", c, x, float64(h[0])/float64(n), float64(h[1])/float64(n))
}

func prob(x, c float64) (l, r float64) {
	l = 0.5 + 0.5*(x/(c+math.Abs(x)))
	r = 1 - l
	return
}
