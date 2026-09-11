// ported from http://iq0.com/duffgram/mort.c

package main

import (
	"flag"
	"fmt"
	"math"
	"os"
	"strconv"
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() != 3 && flag.NArg() != 4 {
		usage()
	}

	a, _ := strconv.ParseFloat(flag.Arg(0), 64)
	r, _ := strconv.ParseFloat(flag.Arg(1), 64)
	t, _ := strconv.ParseFloat(flag.Arg(2), 64)
	r /= 1200
	t *= 12
	c := math.Pow(1+r, t)
	p := a * c * r / (c - 1)
	fmt.Printf("payment=%.2f/month (%.2f/year), total=%.2f multiple=%.4f\n", p, p*12, p*t, p*t/a)
	if flag.NArg() == 4 {
		np, _ := strconv.Atoi(flag.Arg(3))
		c := math.Pow(1+r, float64(np))
		bal := a*c - p*(1-c)/r
		fmt.Printf("Balance after %d payments is %.2f (paid off %.2f)\n", np, bal, a-bal)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: amount rate term [npaid]")
	flag.PrintDefaults()
	os.Exit(2)
}
