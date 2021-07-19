// https://en.wikipedia.org/wiki/Perceptron
package main

import (
	"fmt"
	"math"
)

func main() {
	learn(nand)
	learn(nor)
	learn(and)
	learn(or)
	learn(xnor)
	learn(xor)
}

// single layer perceptron, it can learn the and/or but not xor
func learn(f func(a, b uint) uint) {
	// build a training set
	var x [][]float64
	var d []float64
	for i := uint(0); i < 2; i++ {
		for j := uint(0); j < 2; j++ {
			x = append(x, []float64{1, float64(j), float64(i)})
			d = append(d, float64(f(i, j)))
		}
	}
	// init weight to 0
	w := make([]float64, len(x[0]))
	y := make([]float64, len(d))

	// learning rate
	r := 0.1
	// eror rate
	E := 1e-15
	// iteration before bail if no convergence
	I := int(1e6)
	// initial error
	e := 1.0
	// "pocket" error, best error estimate so far if the function
	// is not linearly separable
	pe := 1.0
	pw := make([]float64, len(w))

	// train until error threshold reached
	// or iteration exceeded
	for i := 0; i < I; i++ {
		// for each example in training set
		for j := range d {
			// binary output so we need to threshold it
			y[j] = dot(w, x[j])
			if y[j] > 0.5 {
				y[j] = 1
			}

			// update the weights
			for i := range w {
				w[i] = w[i] + r*(d[j]-y[j])*x[j][i]
			}
		}

		// check if error rate is low enough
		e = erf(d, y)
		if e < pe {
			pe = e
			copy(pw, w)
		}
		if e < E {
			break
		}
	}

	fmt.Println("error rate:", e)
	fmt.Println("pocket error rate:", pe)
	fmt.Println("weights:", w)
	lf := func(a, b uint, w []float64) uint {
		x := []float64{1, float64(a), float64(b)}
		v := dot(w, x)
		if v > 0.5 {
			v = 1
		}
		return uint(v)
	}
	for i := uint(0); i < 2; i++ {
		for j := uint(0); j < 2; j++ {
			fmt.Printf("%d %d %d %d %d\n", i, j, f(i, j), lf(i, j, w), lf(i, j, pw))
		}
	}
	fmt.Println()
}

// average of the differences in error
func erf(d, y []float64) float64 {
	var v float64
	for i := range d {
		v += math.Abs(d[i] - y[i])
	}
	return (1 / float64(len(d))) * v
}

func dot(w, x []float64) float64 {
	var v float64
	for i := range w {
		v += w[i] * x[i]
	}
	return v
}

func nand(a, b uint) uint {
	return (^(a & b)) & 1
}

func nor(a, b uint) uint {
	return (^(a | b)) & 1
}

func and(a, b uint) uint {
	return (a & b) & 1
}

func or(a, b uint) uint {
	return (a | b) & 1
}

func xnor(a, b uint) uint {
	return ^(a ^ b) & 1
}

func xor(a, b uint) uint {
	return (a ^ b) & 1
}
