/*

https://en.wikipedia.org/wiki/Linear_scale
https://en.wikipedia.org/wiki/Logarithmic_scale

*/

package main

import (
	"flag"
	"fmt"
	"math"
	"os"
	"strconv"
)

var (
	base = flag.Float64("b", 10, "specify base for logspace")
)

func main() {
	flag.Parse()
	if flag.NArg() < 3 {
		usage()
	}

	t0, _ := strconv.ParseFloat(flag.Arg(0), 64)
	t1, _ := strconv.ParseFloat(flag.Arg(1), 64)
	n, _ := strconv.Atoi(flag.Arg(2))
	fmt.Printf("linspace\n")
	fmt.Printf("%v\n", linspace(t0, t1, n))
	fmt.Printf("logspace\n")
	fmt.Printf("%v\n", logspace(t0, t1, n, *base))
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] <start> <end> <samples>")
	flag.PrintDefaults()
	os.Exit(2)
}

func linspace(t0, t1 float64, n int) []float64 {
	if n <= 0 {
		return []float64{}
	}
	if n == 1 {
		return []float64{t0}
	}

	var p []float64
	t := t0
	dt := (t1 - t0) / float64(n-1)
	for i := 0; i < n; i++ {
		p = append(p, t)
		t += dt
	}
	return p
}

func logspace(t0, t1 float64, n int, b float64) []float64 {
	if n <= 0 {
		return []float64{}
	}
	if n == 1 {
		return []float64{math.Pow(b, t0)}
	}

	var p []float64
	t := math.Pow(b, t0)
	dt := math.Pow(b, (t1-t0)/float64(n-1))
	for i := 0; i < n; i++ {
		p = append(p, t)
		t *= dt
	}
	return p
}
