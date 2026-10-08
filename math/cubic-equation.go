// https://en.wikipedia.org/wiki/Cubic_equation
package main

import (
	"fmt"
	"math/cmplx"
)

func main() {
	fmt.Println(cubic(1, -12, 48, -64))
	fmt.Println(cubic(5, -1, 4, -676))
	fmt.Println(cubic(2565, -12, 4.4, -676))
	fmt.Println(cubic(2, 5, 1, -25))
	fmt.Println(cubic(0, 1, 0, -25))
	fmt.Println(cubic(0, 0, 1, 2))
	fmt.Println(cubic(1.4, 5, 1, 2))
	fmt.Println(cubic(0, 0, 2, 0))
	fmt.Println(cubic(0, 0, 0, 5))
	fmt.Println(cubic(0, 5, .2, 5))
	fmt.Println(cubic(0, 0, 5, 2))
}

func linear(a, b complex128) (r []complex128) {
	switch {
	case a == 0 && b == 0: // one real root
		r = []complex128{0}
	case a == 0: // no solution
	default: // one real root
		r = []complex128{-b / a}
	}
	return
}

func quadratic(a, b, c complex128) (r []complex128) {
	d := b*b - 4*a*c
	switch {
	case a == 0: // equation collapsed to linear
		r = linear(b, c)
	case d == 0: // one real root
		r = []complex128{-b / (2 * a)}
	default: // two complex roots
		d = cmplx.Sqrt(d)
		r = []complex128{
			(-b + d) / (2 * a),
			(-b - d) / (2 * a),
		}
	}
	return
}

func cubic(a, b, c, d complex128) (r []complex128) {
	d0 := b*b - 3*a*c
	d1 := 2*b*b*b - 9*a*b*c + 27*a*a*d
	d2 := cmplx.Sqrt(d1*d1 - 4*d0*d0*d0)
	Z := (-1 + cmplx.Sqrt(-3)) / 2
	C := (d1 + d2) / 2
	if C == 0 {
		C = (d1 - d2) / 2
	}

	switch {
	case a == 0: // equation collapsed to quadratic
		r = quadratic(b, c, d)
	case C == 0: // only real root
		r = []complex128{-1 / (3 * a) * b}
	default: // one real root, two complex roots
		C = cmplx.Pow(C, 1.0/3)
		r = []complex128{
			-1 / (3 * a) * (b + C + d0/C),
			-1 / (3 * a) * (b + Z*C + d0/(Z*C)),
			-1 / (3 * a) * (b + Z*Z*C + d0/(Z*Z*C)),
		}
	}
	return
}
