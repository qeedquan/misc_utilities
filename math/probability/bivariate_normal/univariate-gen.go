/*

Generate a bivariate normal distribution given two univariate normal random sources

rho is the variable that controls the correlation of X and Y
rho=0 means uncorrelated (gives a circular plot)
rho=-1/1 means very correlated (gives a line plot)

*/

package main

import (
	"flag"
	"fmt"
	"math"
	"math/rand"
)

var (
	mu1     = flag.Float64("m1", 0, "specify mean 1")
	mu2     = flag.Float64("m2", 0, "specify mean 2")
	sd1     = flag.Float64("s1", 1, "specify stddev 1")
	sd2     = flag.Float64("s2", 1, "specify stddev 2")
	rho     = flag.Float64("rho", 0.3, "specify rho")
	samples = flag.Int("samples", 100000, "number of samples")
)

func main() {
	flag.Parse()
	for range *samples {
		u, v := gen(*mu1, *sd1, *mu2, *sd2, *rho)
		fmt.Printf("%.6f,%.6f\n", u, v)
	}
}

func gen(m1, s1, m2, s2, rho float64) (u, v float64) {
	X := rand.NormFloat64()
	Y := rand.NormFloat64()
	u = m1 + s1*X
	v = m2 + s2*(rho*X+math.Sqrt(1-rho)*Y)
	return
}
