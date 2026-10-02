// https://www.cut-the-knot.org/Curriculum/Combinatorics/JohnsonTrotter.shtml
// https://en.wikipedia.org/wiki/Steinhaus%E2%80%93Johnson%E2%80%93Trotter_algorithm
// generate a permutation of given a initial configuration
package main

import (
	"bufio"
	"flag"
	"fmt"
	"io"
	"math"
	"math/rand"
	"os"
	"time"
)

type dint struct {
	index int
	value int
	dir   bool
}

func main() {
	rand.Seed(time.Now().UnixNano())

	N := 10
	flag.IntVar(&N, "n", N, "permutation size")
	flag.Parse()

	w := bufio.NewWriter(os.Stdout)
	defer w.Flush()

	p := dints(N)
	for i := 1; ; i++ {
		dprint(w, i, p)
		x, y, v := largest(p)
		if x == -1 {
			break
		}
		p[x], p[y] = p[y], p[x]
		reverse(p, v)
	}
}

func dints(n int) []dint {
	p := make([]dint, n)
	for i := range p {
		p[i] = dint{i, rand.Intn(math.MaxInt32), false}
	}
	return p
}

func largest(p []dint) (int, int, int) {
	x, y, v := -1, -1, -1
	for i := range p {
		switch {
		case i > 0 && !p[i].dir && p[i].index > p[i-1].index && p[i].index > v:
			x = i
			y = i - 1
			v = p[i].index
		case i < len(p)-1 && p[i].dir && p[i].index > p[i+1].index && p[i].index > v:
			x = i
			y = i + 1
			v = p[i].index
		}
	}
	return x, y, v
}

func reverse(p []dint, v int) {
	for i := range p {
		if p[i].index > v {
			p[i].dir = !p[i].dir
		}
	}
}

func dprint(w io.Writer, n int, p []dint) {
	fmt.Fprintf(w, "%d [", n)
	for i := range p {
		fmt.Fprintf(w, "%v", p[i].index)
		if i+1 < len(p) {
			fmt.Fprintf(w, " ")
		}
	}
	fmt.Fprintf(w, "]\n")
}
