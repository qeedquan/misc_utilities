package main

import (
	"math"
	"math/cmplx"
)

func main() {
	// tan(t) = sin(t)/cos(t)
	eqf1(
		math.Tan,
		func(t float64) float64 { return math.Sin(t) / math.Cos(t) },
	)

	// sin(2*t) = 2*sin(t)*cos(t)
	eqf1(
		func(t float64) float64 { return math.Sin(2 * t) },
		func(t float64) float64 { return 2 * math.Sin(t) * math.Cos(t) },
	)

	// cos(2*t) = cos(t)^2 - sin(t)^2
	// cos(2*t) = 2*cos(t)^2 - 1
	// cos(2*t) = 1 - 2*sin(t)^2
	eqf1(
		func(t float64) float64 { return math.Cos(2 * t) },
		func(t float64) float64 { s, c := math.Sincos(t); return c*c - s*s },
	)

	eqf1(
		func(t float64) float64 { return math.Cos(2 * t) },
		func(t float64) float64 { c := math.Cos(t); return 2*c*c - 1 },
	)

	eqf1(
		func(t float64) float64 { return math.Cos(2 * t) },
		func(t float64) float64 { s := math.Sin(t); return 1 - 2*s*s },
	)

	// sin(3*t) = 3*sin(t) - 4*sin(t)^3
	eqf1(
		func(t float64) float64 { return math.Sin(3 * t) },
		func(t float64) float64 { s := math.Sin(t); return 3*s - 4*s*s*s },
	)

	// cos(3*t) = 4*cos(t)^3 - 3*cos(t)
	eqf1(
		func(t float64) float64 { return math.Cos(3 * t) },
		func(t float64) float64 { c := math.Cos(t); return 4*c*c*c - 3*c },
	)

	// (exp(i*t) + exp(-i*t)) / 2 = cos(t)
	eqf1(
		func(t float64) float64 {
			z := complex(t, 0)
			return real(cmplx.Exp(1i*z)+cmplx.Exp(-1i*z)) * 0.5
		},
		math.Cos,
	)

	// (exp(-i*t) - exp(i*t)) / 2i = sin(t)
	eqf1(
		func(t float64) float64 {
			z := complex(t, 0)
			return real((cmplx.Exp(-1i*z) - cmplx.Exp(1i*z)) * 0.5i)
		},
		math.Sin,
	)

	// cos(p+q) = cos(p)*cos(q) - sin(p)*sin(q)
	// sin(p+q) = sin(p)*cos(q) + cos(p)*sin(q)
	eqf1(
		func(t float64) float64 {
			p := t
			q := 4.3 * t
			return math.Cos(p + q)
		},
		func(t float64) float64 {
			p := t
			q := 4.3 * t
			sp, cp := math.Sincos(p)
			sq, cq := math.Sincos(q)
			return cp*cq - sp*sq
		},
	)

	eqf1(
		func(t float64) float64 {
			p := t
			q := 5 * t
			return math.Sin(p + q)
		},
		func(t float64) float64 {
			p := t
			q := 5 * t
			sp, cp := math.Sincos(p)
			sq, cq := math.Sincos(q)
			return sp*cq + cp*sq
		},
	)

	// exp(-i*t)*cos(t) = 0.5 + 0.5*exp(-2*i*t)
	eqf1(
		func(t float64) float64 {
			z := complex(t, 0)
			return real(cmplx.Exp(-1i*z)) * math.Cos(t)
		},
		func(t float64) float64 {
			z := complex(t, 0)
			return real(0.5 + 0.5*cmplx.Exp(-2i*z))
		},
	)
}

func assert(x bool) {
	if !x {
		panic("assertion failed")
	}
}

func eqf1(f, g func(float64) float64) {
	const (
		T   = 1e3
		dt  = 1e-3
		eps = 1e-9
	)
	for t := -T; t <= T; t += dt {
		assert(math.Abs(f(t)-g(t)) < eps)
	}
}
