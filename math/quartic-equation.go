// https://en.wikipedia.org/wiki/Quartic_equation

package main

import (
	"fmt"
	"math/cmplx"
)

func main() {
	fmt.Println(quartic(7, 0, 0, 0, 5))
	fmt.Println(quartic(7, 50, 49, 6, 5))
	fmt.Println(quartic(10, 4, 0, 0, 0))
	fmt.Println(quartic(2, 4, 6, 8, 10))
	fmt.Println(quartic(-6, -4, 190, 45, 19))
}

func linear(a, b complex128) (z []complex128) {
	switch {
	case a == 0 && b == 0: // one real root
		z = []complex128{0}
	case a == 0: // no solution
	default: // one real root
		z = []complex128{-b / a}
	}
	return
}

func quadratic(a, b, c complex128) (z []complex128) {
	d := b*b - 4*a*c
	switch {
	case a == 0: // equation collapsed to linear
		z = linear(b, c)
	case d == 0: // one real root
		z = []complex128{-b / (2 * a)}
	default: // two complex roots
		d = cmplx.Sqrt(d)
		z = []complex128{
			(-b + d) / (2 * a),
			(-b - d) / (2 * a),
		}
	}
	return
}

func cubic(a, b, c, d complex128) (z []complex128) {
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
		z = quadratic(b, c, d)
	case C == 0: // only real root
		z = []complex128{-1 / (3 * a) * b}
	default: // one real root, two complex roots
		C = cmplx.Pow(C, 1.0/3)
		z = []complex128{
			-1 / (3 * a) * (b + C + d0/C),
			-1 / (3 * a) * (b + Z*C + d0/(Z*C)),
			-1 / (3 * a) * (b + Z*Z*C + d0/(Z*Z*C)),
		}
	}
	return
}

func quartic(a, b, c, d, e complex128) (z []complex128) {
	// collapsed to cubic
	if a == 0 {
		return cubic(b, c, d, e)
	}

	p := (8*a*c - 3*b*b) / (8 * a * a)
	q := (b*b*b - 4*a*b*c + 8*a*a*d) / (8 * a * a * a)

	d0 := c*c - 3*b*d + 12*a*e
	d1 := 2*c*c*c - 9*b*c*d + 27*b*b*e + 27*a*d*d - 72*a*c*e
	dm := d1*d1 - 4*d0*d0*d0

	// three common roots, one simple root
	if dm == 0 && d0 == 0 {
		r := quadratic(12*a, 6*b, c)
		r0 := r[0]
		r1 := r[1]

		m0 := cmplx.Abs(a*r0*r0*r0*r0 + b*r0*r0*r0 + c*r0*r0 + d*r0 + e)
		m1 := cmplx.Abs(a*r1*r1*r1*r1 + b*r1*r1*r1 + c*r1*r1 + d*r1 + e)

		x0 := r[0]
		if m1 < m0 {
			x0 = r[1]
		}

		x1 := -b/a - 3*x0
		return []complex128{x1, x0, x0, x0}
	}

	// four roots
	dq := d1 * d1
	if dm != 0 && d0 == 0 {
		dq = -dq
	}

	Q := cmplx.Pow(0.5*(d1+cmplx.Sqrt(dq-4*d0*d0*d0)), 1/3.0)
	S := 0.5 * cmplx.Sqrt(-2*p/3+(Q+d0/Q)/(3*a))

	y0 := 0.5 * cmplx.Sqrt(-4*S*S-2*p+q/S)
	y1 := 0.5 * cmplx.Sqrt(-4*S*S-2*p-q/S)

	x := -b / (4 * a)
	z = []complex128{
		x - S + y0,
		x - S - y0,
		x + S + y1,
		x + S - y1,
	}
	return
}
