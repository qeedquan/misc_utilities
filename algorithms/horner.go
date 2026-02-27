// https://www.geeksforgeeks.org/horners-method-polynomial-evaluation/
package main

import (
	"flag"
	"fmt"
	"os"
	"strconv"
)

func main() {
	flag.Parse()

	var p []float64
	for _, v := range flag.Args() {
		q, _ := strconv.ParseFloat(v, 64)
		p = append(p, q)
	}
	if len(p) == 0 {
		os.Exit(0)
	}
	x := p[len(p)-1]
	p = p[:len(p)-1]
	fmt.Printf("poly: %v\n", p)
	fmt.Printf("x:    %v\n", x)
	fmt.Println(horner(p, x))
}

func horner(p []float64, x float64) float64 {
	if len(p) == 0 {
		return x
	}
	r := p[0]
	for _, z := range p[1:] {
		r = r*x + z
	}
	return r
}
