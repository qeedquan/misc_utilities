/*

http://www.site2241.net/november2019.htm
http://www.hyperdynelabs.com/dspdude/papers/DigRadio_w_mathcad.pdf

*/

package main

import (
	"math"
	"math/cmplx"
	"math/rand"
	"time"
)

func main() {
	rand.Seed(time.Now().UnixNano())
	test_basics()
}

/*

A polar discriminator filter is a filter that has the following form

y[n] = x[n]*conj(x[n-1]) where the input x and the output y is a complex value.
arg(y[n]) = phase difference of x[n] and x[n-1]
abs(y[n]) = abs(x[n])*abs(x[n-1])

The output y[n] encodes the magnitude and relative phase difference of x[n] and x[n-1] if x[n] and x[n-1] is viewed as a vector.
The magnitude of y[n] is 1 if both x[n] and x[n-1] are normalized.
The phase difference has sign information to encode whether or it is a clockwise or counterclockwise rotation leading to the identity:

polar_discriminator(x[n], x[n-1]) = -polar_discriminator(x[n-1], x[n])

The phase range is from [-pi, pi]

*/

func pld(x0, x1 complex128) complex128 {
	return x1 * cmplx.Conj(x0)
}

func test_basics() {
	const (
		step = 1e-4
		eps  = 1e-6
	)

	for i := -math.Pi + step; i <= math.Pi; i += step {
		for t := 0.0; t <= 2*math.Pi; t += math.Abs(i) {
			s := complex(100*rand.Float64()+0.1, 0)

			a := s * cmplx.Exp(complex(0, t))
			b := 1 / s * cmplx.Exp(complex(0, t+i))

			x := pld(a, b)
			p := cmplx.Phase(x)
			m := cmplx.Abs(x)

			assert(math.Abs(p-i) < eps)
			assert(math.Abs(1-m) < eps)
			if i == 0 {
				break
			}
		}
	}
}

func assert(x bool) {
	if !x {
		panic("assertion failed")
	}
}
