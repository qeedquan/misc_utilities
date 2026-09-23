/*

https://en.wikipedia.org/wiki/Levinson_recursion
https://docs.scipy.org/doc/scipy/reference/generated/scipy.linalg.solve_toeplitz.html
http://www.emptyloop.com/technotes/A%20tutorial%20on%20linear%20prediction%20and%20Levinson-Durbin.pdf
https://www.kuniga.me/blog/2021/05/13/lpc-in-python.html

Given a finite input signal X(n) with N samples, we want to represent the signal X(n) as:
X[n] = Sum[i=1, M] a[i] * X[n-i] where M is called the order of the filter coefficients (FIR).

That is, we want to represent the signal as a weighted linear combination of its past inputs.
We want extract the "best" linear relationship from what we have seen in the sense that the error squared is minimized.

This algorithm differentiates itself from other least square minimization algorithms is this sense:
The other least square minimization algorithms (LMS/etc) have a desired signal it wants to shape the input signal to, and it does it by finding the optimal weights for the linear combinations;
while this algorithm tries to estimate the current signal output from past input (rather than having an explicit desired signal to work with).

In trying to accomplish this, the problem leads to a matrix that is Toeplitz, which can be solved efficiently using the Levinson Durbin algorithm.
The Levinson Durbin algorithm solves the system of linear equations in O(n^2) for Toeplitz matrix, as opposed to O(n^3) for Gauss-Jordan elimination.

The input signal matters alot on how well this works, but generally more coefficients leads to better fit.
If the input signal can't be approximated well by it's past input (like a random/markovian signal), then the error won't go down that much.

*/

package main

import (
	"fmt"
	"math"
	"math/rand/v2"
)

func main() {
	// example to see how well we can fit using more coefficients
	for n := 1; n <= 1000; n++ {
		x := gen(1000, f1)
		c := fwdldtp(n, x)
		y := predict(c, x)
		fmt.Println(n, compare(c, x, y))
	}
}

func fwdldtp(m int, x []float64) []float64 {
	N := len(x) - 1

	// initialize with autocorrelation coefficients
	R := make([]float64, m+1)
	for i := range R {
		for j := range N - i + 1 {
			R[i] += x[j] * x[j+i]
		}
	}

	A := make([]float64, m+1)
	A[0] = 1
	E := R[0]
	for k := range m {
		L := 0.0
		for j := range k + 1 {
			L -= A[j] * R[k+1-j]
		}
		L /= E

		for n := range (k+1)/2 + 1 {
			A[n], A[k+1-n] = A[n]+L*A[k+1-n], A[k+1-n]+L*A[n]
		}

		E *= 1 - L*L
	}
	return A[1:]
}

func predict(c, x []float64) []float64 {
	p := make([]float64, len(x))
	for i := len(c); i < len(p); i++ {
		for j := range len(c) {
			p[i] -= c[j] * x[i-1-j]
		}
	}
	return p
}

func compare(c, x, y []float64) float64 {
	e := 0.0
	for i := len(c); i < len(x); i++ {
		d := y[i] - x[i]
		e += d * d
	}
	return e
}

func gen(n int, f func(float64) float64) []float64 {
	r := make([]float64, n)
	for i := range r {
		r[i] = f(float64(i))
	}
	return r
}

func f1(t float64) float64 {
	return math.Sin(t*0.01) + 0.75*math.Sin(t*0.03) + 0.5*math.Sin(t*0.05) + 0.25*math.Sin(t*0.11)
}

func f2(x float64) float64 {
	return x
}

func f3(x float64) float64 {
	return 3
}

func f4(x float64) float64 {
	return math.Pow(x*0.001, 9)
}

func f5(x float64) float64 {
	return math.Exp(x * 0.00001)
}

func f6(x float64) float64 {
	return rand.Float64() * x
}
