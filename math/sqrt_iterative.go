package main

import (
	"flag"
	"fmt"
	"math"
	"strconv"
)

func main() {
	var limit = 100.0
	flag.Parse()
	if flag.NArg() >= 1 {
		limit, _ = strconv.ParseFloat(flag.Arg(0), 64)
	}
	for i := 1.0; i <= limit; i++ {
		x, s := sqrt(i)
		fmt.Printf("%v iterations: %v %v\n", s, i, x)
	}
}

func sqrt(x float64) (float64, int) {
	const eps = 0.000001
	p := 1.0
	for i := 0; ; i++ {
		if math.Abs(x-p*p) < eps {
			return p, i
		}
		p = (p + x/p) / 2
	}
}
