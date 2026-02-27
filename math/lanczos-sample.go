package main

import (
	"flag"
	"fmt"
	"log"
	"math"
	"math/rand"
	"os"
	"strconv"
	"time"
)

func main() {
	rand.Seed(time.Now().UnixNano())
	flag.Usage = usage
	flag.Parse()
	A := -100.0
	B := 100.0
	N := 100
	D := 1e-2
	switch flag.NArg() {
	case 3:
		B, _ = strconv.ParseFloat(flag.Arg(2), 64)
		fallthrough
	case 2:
		A, _ = strconv.ParseFloat(flag.Arg(1), 64)
		fallthrough
	case 1:
		N, _ = strconv.Atoi(flag.Arg(0))
	}
	if B < A {
		A, B = B, A
	}

	S := gen(A, B, N)
	T := sgen(S, D)

	r := len(T) / len(S)
	n := 0
	w := os.Stdout
	for i := range T {
		fmt.Fprintln(w, i, T[i], S[n%len(S)])
		if i%r == 0 {
			n++
		}
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: lanczos-sample <num_samples> <start> <end>")
	flag.PrintDefaults()
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func gen(a, b float64, n int) []float64 {
	p := make([]float64, n)
	for i := 0; i < n; i++ {
		p[i] = lerp(rand.Float64(), a, b)
	}
	return p
}

func sgen(s []float64, D float64) []float64 {
	var p []float64
	for i := 0.0; i < float64(len(s)); i += D {
		p = append(p, sample(s, i))
	}
	return p
}

func sample(s []float64, x float64) float64 {
	const A = 3

	f := int(math.Floor(x))
	a := f - A + 1
	b := f + A
	if a < 0 {
		a = 0
	}
	if b >= len(s) {
		b = len(s) - 1
	}

	z := 0.0
	for i := a; i <= b; i++ {
		z += s[i] * lanczos(x-float64(i), float64(i))
	}
	return z
}

func lanczos(x, a float64) float64 {
	if x == 0 {
		return 1
	}
	return sinc(x) * sinc(x/a)
}

func sinc(x float64) float64 {
	return math.Sin(math.Pi*x) / (math.Pi * x)
}

func lerp(x, a, b float64) float64 {
	return a*x + (1-x)*b
}
