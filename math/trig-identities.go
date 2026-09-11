/*

https://en.wikipedia.org/wiki/List_of_trigonometric_identities
https://iquilezles.org/articles/trigfunctions/
https://www.crewes.org/Documents/SlideShows/2017/CSS201730.pdf

*/

package main

import (
	"math"
	"math/cmplx"
	"math/rand/v2"
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

	// (exp(x) - exp(-x)) / 2 = sinh(x)
	eqf1r(
		func(x float64) float64 {
			return (math.Exp(x) - math.Exp(-x)) / 2
		},
		func(x float64) float64 {
			return math.Sinh(x)
		},
		100,
	)

	// (exp(x) + exp(-x)) / 2 = cosh(x)
	eqf1r(
		func(x float64) float64 {
			return (math.Exp(x) + math.Exp(-x)) / 2
		},
		func(x float64) float64 {
			return math.Cosh(x)
		},
		100,
	)

	// (exp(x) - exp(-x)) / (exp(x) + exp(-x)) = tanh(x)
	eqf1r(
		func(x float64) float64 {
			return (math.Exp(x) - math.Exp(-x)) / (math.Exp(x) + math.Exp(-x))
		},
		func(x float64) float64 {
			return math.Tanh(x)
		},
		100,
	)

	// Used in QAM modulation, appear often in minima/maxima of different SDF shapes
	// a*cos(x) + b*sin(x) = sqrt(a^2 + b^2) * cos( x - atan(b/a))
	a := rand.Float64() * 1e3
	b := rand.Float64() * 1e3
	eqf1(
		func(x float64) float64 {
			return a*math.Cos(x) + b*math.Sin(x)
		},
		func(x float64) float64 {
			return math.Hypot(a, b) * math.Cos(x-math.Atan(b/a))
		},
	)

	// a*cos(x)^2 + b*sin(x)^2 = (a + b)/2 + (a - b)/2 * cos(2*x)
	a = rand.Float64() * 1e3
	b = rand.Float64() * 1e3
	eqf1(
		func(x float64) float64 {
			return a*math.Cos(x)*math.Cos(x) + b*math.Sin(x)*math.Sin(x)
		},
		func(x float64) float64 {
			return (a+b)/2 + (a-b)/2*math.Cos(2*x)
		},
	)

	// Half angle trigonometric vs radicals
	// These appear often when a quadratic quantity is lurking around.
	// They are related to the trisect() function at the core of cubic solvers, but one degree less.

	// sin(asin(x) / 2) = sqrt((1 - sqrt(1 - x^2)) / 2) * sign(x)
	eqf1(
		func(x float64) float64 {
			return math.Sin(math.Asin(x) / 2)
		},
		func(x float64) float64 {
			return math.Sqrt((1-math.Sqrt(1-x*x))/2) * sign(x)
		},
	)

	// cos(asin(x) / 2) = sqrt((1 + sqrt(1 - x^2)) / 2)
	eqf1(
		func(x float64) float64 {
			return math.Cos(math.Asin(x) / 2)
		},
		func(x float64) float64 {
			return math.Sqrt((1 + math.Sqrt(1-x*x)) / 2)
		},
	)

	// sin(atan(x) / 2) = sqrt((1 - 1/sqrt(1 + x^2)) / 2)
	eqf1(
		func(x float64) float64 {
			return math.Abs(math.Sin(math.Atan(x) / 2))
		},
		func(x float64) float64 {
			return math.Sqrt((1 - 1/math.Sqrt(1+x*x)) / 2)
		},
	)

	// cos(atan(x) / 2) = sqrt((1 + 1/sqrt(1 + x^2)) / 2)
	eqf1(
		func(x float64) float64 {
			return math.Abs(math.Cos(math.Atan(x) / 2))
		},
		func(x float64) float64 {
			return math.Sqrt((1 + 1/math.Sqrt(1+x*x)) / 2)
		},
	)

	// sin(acos(x) / 2) = sqrt((1 - x) / 2)
	eqf1r(
		func(x float64) float64 {
			return math.Sin(math.Acos(x) / 2)
		},
		func(x float64) float64 {
			return math.Sqrt((1 - x) / 2)
		},
		1,
	)

	// cos(acos(x) / 2) = sqrt((1 + x) / 2)
	eqf1r(
		func(x float64) float64 {
			return math.Cos(math.Acos(x) / 2)
		},
		func(x float64) float64 {
			return math.Sqrt((1 + x) / 2)
		},
		1,
	)

	// example function that can be learned by LMS/Gradient Descent Methods
	// p = constant random phase
	// x1 = sin(x)            | sin(2*pi*k/N)
	// x2 = sin(x - p)        | sin(2*pi*(k-n)/N)
	// w1 = 2*cot(p)          | 2*cot(2*pi*n/N)
	// w2 = -2*csc(p)         | -2*csc(2*pi*n/N)
	// x1*w1 + w2*w2 = cos(x) | 2*cos(2*pi*k/N)
	p := rand.Float64()
	eqf1r(
		func(x float64) float64 {
			x1 := math.Sin(x)
			x2 := math.Sin(x - p)
			w1 := 2 / math.Tan(p)
			w2 := -2 / math.Sin(p)
			return x1*w1 + x2*w2
		},
		func(x float64) float64 {
			return 2 * math.Cos(x)
		},
		2*math.Pi,
	)
}

func assert(x bool) {
	if !x {
		panic("assertion failed")
	}
}

func eqf1(f, g func(float64) float64) {
	eqf1r(f, g, 1e3)
}

func eqf1r(f, g func(float64) float64, T float64) {
	const (
		dt  = 1e-3
		eps = 1e-8
	)
	for t := -T; t <= T; t += dt {
		x := f(t)
		y := g(t)
		if math.IsNaN(x) && math.IsNaN(y) {
			continue
		}
		assert(math.Abs(x-y) < eps)
	}
}

func sign(x float64) float64 {
	if x < 0 {
		return -1
	}
	if x == 0 {
		return 0
	}
	return 1
}
