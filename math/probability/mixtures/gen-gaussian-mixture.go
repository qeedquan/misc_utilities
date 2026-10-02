package main

import (
	"flag"
	"fmt"
	"math/rand"
	"os"
	"strconv"
)

func main() {
	mu := flag.Float64("mean", 1000, "mean scale")
	sd := flag.Float64("stddev", 100, "standard deviation scale")
	m := flag.Int("mixtures", 1, "number of mixtures")

	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	s, _ := strconv.Atoi(flag.Arg(0))
	p := newmixtures(*m, *mu, *sd)
	for _, p := range p {
		fmt.Printf("# weight=%.6f mean=%.6f stddev=%.6f\n", p.w, p.mu, p.sd)
	}

	for i := 0; i < s; i++ {
		fmt.Printf("%v\n", gen(p))
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] <samples>")
	flag.PrintDefaults()
	os.Exit(2)
}

type Mixture struct {
	w  float64
	mu float64
	sd float64
}

func gen(m []Mixture) float64 {
	r := 0.0
	x := rand.Float64()
	for _, p := range m {
		if x < p.w {
			r = rand.NormFloat64()*p.sd + p.mu
			break
		}
		x -= p.w
	}
	return r
}

func newmixtures(n int, mu, sd float64) []Mixture {
	m := make([]Mixture, n)
	t := 0.0
	for i := range m {
		m[i].w = rand.Float64()
		m[i].mu = rand.Float64() * mu
		m[i].sd = rand.Float64() * sd

		t += m[i].w
	}
	for i := range m {
		m[i].w /= t
	}
	return m
}
