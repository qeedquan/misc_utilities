// https://en.wikipedia.org/wiki/Complex_number
package main

import (
	"fmt"
	"math"
	"math/cmplx"
	"math/rand"
	"time"
)

type Complex struct {
	R, I float64
}

func (a Complex) Add(b Complex) Complex {
	return Complex{a.R + b.R, a.I + b.I}
}

func (a Complex) Sub(b Complex) Complex {
	return Complex{a.R - b.R, a.I - b.I}
}

func (a Complex) Mul(b Complex) Complex {
	return Complex{
		a.R*b.R - a.I*b.I,
		a.R*b.I + a.I*b.R,
	}
}

func (a Complex) Quo(b Complex) Complex {
	iz := 1 / (b.R*b.R + b.I*b.I)
	return Complex{
		iz * (a.R*b.R + a.I*b.I),
		iz * (a.I*b.R - a.R*b.I),
	}
}

func (a Complex) Reciprocal() Complex {
	return a.Quo(a.Mul(a))
}

func (a Complex) Conj() Complex {
	return Complex{a.R, -a.I}
}

func (a Complex) Complex128() complex128 {
	return complex(a.R, a.I)
}

func (a Complex) Polar() Polar {
	return Polar{
		math.Sqrt(a.R*a.R + a.I*a.I),
		math.Atan2(a.I, a.R),
	}
}

func (a Complex) Abs() float64 {
	return math.Sqrt(a.R*a.R + a.I*a.I)
}

func (a Complex) Equals(b Complex, eps float64) bool {
	return math.Abs(a.R-b.R) <= eps && math.Abs(a.I-b.I) <= eps
}

func (a Complex) String() string {
	return fmt.Sprintf("(%f%+fi)", a.R, a.I)
}

// polar doesn't have simple add/sub formulas
// so way to do it is convert it to rectangle coordinates
// do the add, and then convert it back
type Polar struct {
	R, T float64
}

func (a Polar) Mul(b Polar) Polar {
	return Polar{
		a.R * b.R,
		a.T + b.T,
	}
}

func (a Polar) Quo(b Polar) Polar {
	return Polar{
		a.R / b.R,
		a.T - b.T,
	}
}

func (a Polar) Equals(b Polar, eps float64) bool {
	return math.Abs(a.R-b.R) <= eps && math.Abs(a.T-b.T) <= eps
}

func (a Polar) Reciprocal() Polar {
	return Polar{1 / a.R, -a.T}
}

func (a Polar) Complex() Complex {
	s, c := math.Sincos(a.T)
	return Complex{
		a.R * c,
		a.R * s,
	}
}

func main() {
	rand.Seed(time.Now().UnixNano())
	testArithmetic()
	testCommutative()
	testPolar()
	testEuler()
	testAbs()
	testConjugate()
	testPowerSeriesUnit()
}

func randRealVector(size int, scale float64) []float64 {
	p := make([]float64, size)
	for i := range p {
		p[i] = rand.Float64() * scale
	}
	return p
}

func testArithmetic() {
	for i := 0; i < 1e6; i++ {
		a := randComplex(math.MaxFloat32)
		b := randComplex(math.MaxFloat32)

		c := a.Complex128()
		d := b.Complex128()

		x := a.Add(b)
		y := c + d
		if !equalComplex(x, y) {
			fmt.Println("add mismatch:", a, b, c, d, x, y)
		}

		x = a.Sub(b)
		y = c - d
		if !equalComplex(x, y) {
			fmt.Println("sub mismatch:", a, b, c, d, x, y)
		}

		x = a.Mul(b)
		y = c * d
		if !equalComplex(x, y) {
			fmt.Println("mul mismatch:", a, b, c, d, x, y)
		}

		x = a.Quo(b)
		y = c / d
		if !equalComplex(x, y) {
			fmt.Println("quo mismatch:", a, b, c, d, x, y)
		}

		a = randComplex(1e7)
		x = a.Mul(a.Reciprocal())
		if !x.Equals(Complex{1, 0}, 1e-6) {
			fmt.Println("reciprocal mismatch: ", a, x)
		}
	}
}

// complex multiplication is commutative
// we can see this fact geometrically because
// complex multiplication represents a rotation in 2d space,
// where it is known that 2d rotation are commutative
// (higher dimension like 3d are not commutative though)
// this applies with polar coordinates too
func testCommutative() {
	for i := 0; i < 1e6; i++ {
		a := randComplex(1e5)
		b := randComplex(1e4)

		x := a.Mul(b)
		y := b.Mul(a)
		if !x.Equals(y, 1e-5) {
			fmt.Println("mul not commutative", x, y)
		}

		p := x.Polar()
		q := y.Polar()

		u := p.Mul(q)
		v := q.Mul(p)
		if u != v {
			fmt.Println("mul not commutative", x, y)
		}
	}
}

func testPolar() {
	for i := 0; i < 1e6; i++ {
		a := randComplex(1e5)
		b := randComplex(1e5)

		p := a.Polar()
		q := b.Polar()

		x := a.Mul(b)
		y := p.Mul(q).Complex()
		if !x.Equals(y, 1e-5) {
			fmt.Println("polar mul mismatch:", x, y)
		}

		x = a.Quo(b)
		y = p.Quo(q).Complex()
		if !x.Equals(y, 1e-5) {
			fmt.Println("polar quo mismatch:", x, y)
		}

		x = a.Mul(a.Reciprocal())
		if !x.Equals(Complex{1, 0}, 1e-6) {
			fmt.Println("polar reciprocal mismatch", a, x)
		}

		r := rand.Float64() * 100
		t := rand.Float64() * 2 * math.Pi
		a = Complex{r * math.Cos(t), r * math.Sin(t)}
		p = a.Polar()
		x = a.Mul(a)
		y = p.Mul(p).Complex()
		if !x.Equals(y, 1e-5) {
			fmt.Println("polar mul sin/cos mismatch:", x, y)
		}
	}
}

func testEuler() {
	for i := 0; i < 1e5; i++ {
		v := randComplex(1e2).Complex128()
		c := cmplx.Cos(v)
		s := cmplx.Sin(v)

		// for complex numbers
		// cos(z) = (e^(iz) + e^(-iz)) / 2
		// sin(z) = (e^(iz) - e^(-iz)) / 2
		x := (cmplx.Exp(v*1i) + cmplx.Exp(-v*1i)) / 2
		y := (cmplx.Exp(v*1i) - cmplx.Exp(-v*1i)) / (2 * 1i)
		if !equalCmplx(c, x) || !equalCmplx(y, s) {
			fmt.Println("euler sin/cos complex mismatch", x, y, c, s)
		}

		// for real numbers
		// cos(x) + isin(x) = exp(ix)
		u := rand.Float64() * 1e3
		x = complex(math.Cos(u), math.Sin(u))
		y = cmplx.Exp(complex(0, u))
		if !equalCmplx(x, y) {
			fmt.Println("euler sin/cos real mismatch", u, x, y)
		}
	}

	// e^(i*pi) + 1 = 0
	x := complex(math.Cos(math.Pi), math.Sin(math.Pi)) + 1
	y := cmplx.Exp(complex(0, math.Pi)) + 1
	if !equalCmplx(x, 0) || !equalCmplx(y, 0) {
		fmt.Println("euler identity mismatch:", x, y)
	}

	// e^(i*x) * e^(i*y) = e^(i*(x+y))
	x = cmplx.Exp(0)
	y = cmplx.Exp(0)
	for i := 1; i < 1e2; i++ {
		x = cmplx.Exp(complex(0, float64(i)))
		y = y * cmplx.Exp(0+1i)
		if !equalCmplx(x, y) {
			fmt.Println("euler exp add mismatch", x, y)
		}
	}
}

func testAbs() {
	// abs(z*z') = abs(z)*abs(z')
	for i := 0; i < 1e3; i++ {
		z := randComplex(1e3)
		zp := z.Conj()

		x := z.Mul(zp).Abs()
		y := z.Abs() * zp.Abs()
		if math.Abs(x-y) >= 1e-6 {
			fmt.Println("abs mul mismatch", x, y, math.Abs(x-y))
		}
	}
}

func randComplex(mag float64) Complex {
	return Complex{
		rand.Float64() * mag,
		rand.Float64() * mag,
	}
}

func equalComplex(a Complex, b complex128) bool {
	const eps = 1e-6
	return math.Abs(a.R-real(b)) <= eps && math.Abs(a.I-imag(b)) <= eps
}

func equalCmplx(a, b complex128) bool {
	const eps = 1e-6
	return real(a)-real(b) <= eps && imag(a)-imag(b) <= eps
}

func testConjugate() {
	// e^(-i0) + e(i0) always cancel the imaginary part out (leaves only the real part intact)
	// this is crucial for quadrature signals
	// http://www.dspguru.com/files/QuadSignals.pdf
	for f := 0.0; f <= 100; f += 1.0 {
		for t := 0.0; t <= 1000; t += 0.01 {
			theta := 2 * math.Pi * t * f
			z := cmplx.Exp(complex(0, theta))
			zi := cmplx.Exp(complex(0, -theta))
			rp := z + zi
			if imag(rp) != 0 {
				fmt.Println("cancellation failed", rp)
			}
		}
	}
}

// coefficients is an array contains a_0, a_1, a_2, ... a_n and this function
// evaluate sum[a_k * s^k], where s = i * 2 * pi * freq, k = 0 ... n
func testPowerSeriesUnit() {
	// these power series usually arises in fourier/laplace transforms
	// and we want to evaluate them, here are a few ways to do it
	for i := 0; i < 100; i++ {
		p := randRealVector(5, 4)
		f := rand.Float64() * 100

		a := evalPowerSeriesUnit1(p, f)
		b := evalPowerSeriesUnit2(p, f)
		if !equalCmplx(a, b) {
			fmt.Println("power series unit mismatch", f, a, b)
		}
	}
}

func evalPowerSeriesUnit1(coeffs []float64, freq float64) complex128 {
	omega := 2 * math.Pi * freq
	im := 0.0
	re := 0.0

	s := 1.0
	for order := range coeffs {
		// i^0 = 1, i^1 = i, i^2 = -1, i^3 = -i
		switch order & 3 {
		case 0:
			re += s * coeffs[order]
		case 1:
			im += s * coeffs[order]
		case 2:
			re -= s * coeffs[order]
		case 3:
			im -= s * coeffs[order]
		}
		s *= omega
	}
	return complex(re, im)
}

func evalPowerSeriesUnit2(coeffs []float64, freq float64) complex128 {
	r := complex(0, 0)
	p := complex(0, 2*math.Pi*freq)
	s := complex(1, 0)
	for order := range coeffs {
		r += complex(coeffs[order], 0) * s
		s *= p
	}
	return r
}
