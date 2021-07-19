// http://mathfaculty.fullerton.edu/mathews/n2003/montecarlopimod.html
// calculate pi using monte carlo method

// the ratio of the area of circle and area of square is
// pi*r**2 / (2*r)**2 = pi/4 so the ratio is
// p=pi/4, solving for pi we get pi=p*4

// we can throw points randomly in the square and see
// if it is inside the circle, then the ratio of points inside the circle
// divided the number of points we throw in total approximates
// area of circle / area of square then if we multiply by 4 we should get
// an approximation of pi

package main

import (
	"flag"
	"fmt"
	"math/rand"
	"os"
	"time"
)

var (
	rounds = flag.Int("n", 1e7, "number of rounds")
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if *rounds <= 0 {
		usage()
	}

	rand.Seed(time.Now().UnixNano())
	fmt.Println(mcpi(*rounds))
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options]")
	flag.PrintDefaults()
	os.Exit(2)
}

func mcpi(n int) float64 {
	var in float64
	for i := 0; i < n; i++ {
		x := rand.Float64()
		y := rand.Float64()
		d := x*x + y*y
		if d <= 1 {
			in++
		}
	}
	return 4 * float64(in) / float64(n)
}
