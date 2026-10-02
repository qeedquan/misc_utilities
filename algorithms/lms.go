/*

https://en.wikipedia.org/wiki/Least_mean_squares_filter
https://www.seas.upenn.edu/~cis520/lectures/perceptrons.pdf
https://web.ece.ucsb.edu/~yoga/courses/Adapt/P6_Adaptive_Filtering_LMS%20.pdf
https://personal.utdallas.edu/~saquib/EE6365/1srorder-convergence-of-lms.pdf
https://docs.scipy.org/doc/scipy/reference/generated/scipy.signal.lfilter.html
https://www.youtube.com/watch?v=hc2Zj55j1zU
https://www.youtube.com/watch?v=skfNlwEbqck
https://www.youtube.com/watch?v=fWP8w00rlhY

The LMS algorithm is an adaptive filter that finds FIR coefficients for systems that can be represented (or approximated) as:

f = x1*h1 + x2*h2 + ... xn*hn
x = input signal
h = filter coefficients
n = number of taps (coefficients)

The finite impulse response (FIR) least mean squares filter is related to the Wiener filter, but minimizing the error criterion of the former does not rely on cross-correlations or auto-correlations.
Its solution converges to the Wiener filter solution.

This algorithm can adjust the filter coefficients as new signal inputs so it can adapt to piecewise signals (assuming the signal last long enough for it to adapt)
If one wants a fixed filter rather than adaptive, feed a training sequence to the algorithm to derive the coefficients and use that until the next training sequence is applied.

For system identification:
If a signal is representable by a FIR filter, then LMS can figure out the FIR coefficients to high accuracy.
If a signal is representable by a IIR filter, then the accuracy depends on the number of taps (more taps is higher accuracy).

The learning rate (mu) is a value between [0, 1] (LMS blows up outside the range) controls how fast or slow it adapts to new data.
There are various strategies on how the learning rate can be set:
1. Use a large learning rate to lock on the signal before using a small learning rate to stabilize.
2. Use fixed learning rate that works (by tuning it and seeing how well it works with the particular system).

The learning rate also depends on the number of samples one has to work with.
If the signal doesn't have many samples captured, then high learning rate has to be used, since a low learning rate won't converge quickly enough before one runs out of samples to use.

One can think of LMS as an online learning method that tries to minimize the following:
Least mean squares (LMS) - Online regression -- L2 error
Perceptron - Online SVM -- Hinge loss

As the LMS algorithm does not use the exact values of the expectations, the weights would never reach the optimal weights in the absolute sense, but a convergence is possible in mean.
That is, even though the weights may change by small amounts, it changes about the optimal weights.
However, if the variance with which the weights change, is large, convergence in mean would be misleading. This problem may occur, if the value of step-size  mu is not chosen properly.
An upper bound on mu is given as 0 < mu < 2/lambda_max where lambda_max is the greatest eigenvalue of the autocorrelation matrix R(x) = E[x(n) * x^H(n)
If this condition is not fulfilled, the algorithm becomes unstable and h_hat(n) diverges.
Maximum convergence speed is achieved when mu = 2 / (lambda_max + lambda_min) where lambda_min is the smallest eigenvalue of R.

Normalized least mean squares filter (NLMS):
The main drawback of the "pure" LMS algorithm is that it is sensitive to the scaling of its input x(n)
This makes it very hard (if not impossible) to choose a learning rate mu that guarantees the stability of the algorithm.
The Normalised least mean squares filter (NLMS) is a variant of the LMS algorithm that solves this problem by normalising with the power of the input.
The NLMS algorithm adds a normalizing factor to normalize the power of the input.

LMS algorithm:
Parameters:
p = filter order
mu = step size (learning rate)

Init:
# Initialize the filter to zeros at the start, over time it should adapt to the correct values
h(0) = zeros(p)

Computation: for n = 0, 1, 2 ...

# Window of the input signal (length is the size of the h[n] filter)
x[n]

# Desired signal (assume we know the desired output and want to transform the input signal to looking like the desired)
d[n]

# Compute the error of desired vs the input signal times filter
e[n] = d[n] - x[n]*h[n]

# Update the filter coefficients based on the error signal
h[n + 1] = h[n] + [mu * e*[n] * x[n]]

# For normalize least mean squares, the update filter adds a normalizing term:
h[n + 1] = h[n] + [mu * e*[n] * x[n]] / (x_h[n]*x[n])

*/

package main

import (
	"fmt"
	"math/rand/v2"
)

func main() {
	// Example of LMS finding the FIR coefficients for an input signal
	// Generate a random input signal X and a random filter FIR H_UNK
	// Generate a desired response from convolving the filter X with H_UNK
	// Run the LMS algorithm, the filter LMS converges on should closely match the random filter H_UNK
	order := 50
	X := randvector(10000, 100)
	H_UNK := randvector(order, 1)
	R := filter(H_UNK, []float64{1}, X)
	H := findfilter(X, R, 0.2, order)
	for i := range H {
		fmt.Println(H[i], H_UNK[i])
	}
}

func findfilter(x, r []float64, mu float64, order int) []float64 {
	h := make([]float64, order)
	for n := order; n < len(x); n++ {
		D := reverse(x[n-order : n])
		yn := dot(h, D)
		en := yn - r[n-1]

		s := (2 * mu * en)
		// normalizing factor
		s /= dot(D, D)

		h = sub(h, scale(D, s))
	}
	return h
}

// https://stackoverflow.com/questions/17506343/how-can-i-write-the-matlab-filter-function-myself
func filter(b, a, x []float64) []float64 {
	if len(a) == 0 || len(b) == 0 || len(x) == 0 {
		return []float64{}
	}

	a1 := scale(a, 1/a[0])
	b1 := scale(b, 1/a[0])
	r := make([]float64, len(x))
	r[0] = b1[0] * x[0]
	for i := 1; i < len(x); i++ {
		r[i] = 0.0
		for j := 0; j <= i; j++ {
			k := i - j
			if j > 0 {
				if k < len(b1) && j < len(x) {
					r[i] += b1[k] * x[j]
				}
				if k < len(r) && j < len(a1) {
					r[i] -= a1[j] * r[k]
				}
			} else {
				if k < len(b1) && j < len(x) {
					r[i] += (b1[k] * x[j])
				}
			}
		}
	}
	return r
}

func randvector(n int, scale float64) []float64 {
	x := make([]float64, n)
	for i := range x {
		x[i] = rand.Float64() * scale
	}
	return x
}

func sub(x, y []float64) []float64 {
	r := make([]float64, len(x))
	for i := range r {
		r[i] = x[i] - y[i]
	}
	return r
}

func scale(x []float64, s float64) []float64 {
	r := make([]float64, len(x))
	for i := range r {
		r[i] = x[i] * s
	}
	return r
}

func reverse(x []float64) []float64 {
	r := make([]float64, len(x))
	for i := range x {
		r[len(r)-i-1] = x[i]
	}
	return r
}

func dot(x, y []float64) float64 {
	r := 0.0
	for i := range x {
		r += x[i] * y[i]
	}
	return r
}
