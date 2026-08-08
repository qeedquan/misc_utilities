package main

import (
	"fmt"
	"math"
	"math/rand"
)

func main() {
	x := []float64{2, 4, 4, 4, 5, 5, 7, 9}
	fmt.Println(average(x), variance(x), variance2(x), sampstddev(x), compstddev(x))

	for i := 0; i < 10; i++ {
		x := randn(20)
		fmt.Println(variance(x), variance2(x))
	}
}

func randn(n int) []float64 {
	x := make([]float64, n)
	for i := range x {
		x[i] = rand.Float64()
	}
	return x
}

func average(x []float64) float64 {
	if len(x) == 0 {
		return 0
	}

	u := 0.0
	for i := range x {
		u += x[i]
	}
	return u / float64(len(x))
}

func variance(x []float64) float64 {
	if len(x) == 0 {
		return 0
	}

	u := average(x)
	r := 0.0
	for i := range x {
		r += (x[i] - u) * (x[i] - u)
	}
	return r / float64(len(x))
}

func variance2(x []float64) float64 {
	if len(x) == 0 {
		return 0
	}

	u := 0.0
	v := 0.0
	for i := range x {
		u += x[i]
		v += x[i] * x[i]
	}
	n := float64(len(x))
	u /= n
	v /= n
	return v - u*u
}

// sample stddev is not the same as complete stddev
// since we divide by n-1 rather than n
func sampstddev(x []float64) float64 {
	if len(x) <= 1 {
		return 0
	}

	u := average(x)
	r := 0.0
	for i := range x {
		r += (x[i] - u) * (x[i] - u)
	}
	return math.Sqrt(r / float64(len(x)-1))
}

// complete standard deviation
func compstddev(x []float64) float64 {
	return math.Sqrt(variance(x))
}
