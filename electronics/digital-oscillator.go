// https://basesandframes.files.wordpress.com/2020/04/even_faster_math_functions_green_2020.pdf
// http://www.claysturner.com/dsp/digital_resonators.pdf
// http://math.colgate.edu/~wweckesser/math312Spring06/handouts/IMM_2x2linalg.pdf
// https://www.scss.tcd.ie/Rozenn.Dahyot/CS1BA1/SolutionGeometricalTransformations.pdf
package main

import (
	"bufio"
	"fmt"
	"math"
	"math/cmplx"
	"os"
)

func main() {
	testprop()

	// the initial seed is important for wavelike behavior
	// if we use bad one, they become a line or a point

	// this one is a point
	// testosc(0, 0, 204, 5e2)

	// testosc(.2, .13, .2, 5e2)
	testosc(math.Cos(0), math.Sin(0), 1/(2*math.Pi), 5e2)
}

func assert(x bool) {
	if !x {
		panic("assertion failed")
	}
}

func testprop() {
	const eps = 1e-8

	tab := [][3]float64{
		{0, 2 * math.Pi, 1 / (2 * math.Pi)},
		{4535, 425421, 25986846},
		{4591, 4836, 284.2},
		{3, 18, 2 * math.Pi},
	}
	for _, t := range tab {
		for i := t[0]; i <= t[1]; i += t[2] {
			for _, o := range osctypes {
				m := genosc(o, i)

				// all oscillators should have determinant of 1
				// no matter what theta is
				assert(math.Abs(det(m)-1) < eps)

				// all oscillator should have complex eigenvalues
				// in which their absolute value is less than or equal to 2
				e := eig(m)
				assert(cmplx.Abs(e[0]) <= 2)
				assert(cmplx.Abs(e[1]) <= 2)
			}
		}
	}
}

func testosc(a, b, t float64, n int) {
	var osc [OSC_MAX][2][2]float64
	var in, out [OSC_MAX][2]float64

	for i := range osctypes {
		osc[i] = genosc(osctypes[i], t)
	}

	for i := range in {
		in[i] = [2]float64{a, b}
	}

	w := bufio.NewWriter(os.Stdout)
	defer w.Flush()
	for i := 0; i < n; i++ {
		for j := range in {
			fmt.Fprintf(w, "%.6f ", in[j][0])
		}
		fmt.Fprintf(w, "\n")

		for j := range in {
			out[j] = mul(osc[j], in[j])
		}
		in = out
	}
}

func det(m [2][2]float64) float64 {
	return m[0][0]*m[1][1] - m[1][0]*m[0][1]
}

func tr(m [2][2]float64) float64 {
	return m[0][0] + m[1][1]
}

func eig(m [2][2]float64) [2]complex128 {
	t := complex(tr(m), 0)
	d := complex(det(m), 0)
	r := cmplx.Sqrt(t*t-4*d) / 2
	return [2]complex128{t + r, t - r}
}

func mul(m [2][2]float64, v [2]float64) [2]float64 {
	return [2]float64{
		m[0][0]*v[0] + m[0][1]*v[1],
		m[1][0]*v[0] + m[1][1]*v[1],
	}
}

var osctypes = [OSC_MAX]int{
	OSC_COUPLED_QUADRATURE,
	OSC_MAGIC_CIRCLE,
	OSC_QUADRATURE_STAGGERED,
	OSC_BIQUAD,
	OSC_DIGITAL_WAVEGUIDE,
	OSC_REINSCH,
	OSC_TYPE_A,
	OSC_TYPE_B,
	OSC_TYPE_C,
}

const (
	OSC_COUPLED_QUADRATURE = iota
	OSC_MAGIC_CIRCLE
	OSC_QUADRATURE_STAGGERED
	OSC_BIQUAD
	OSC_DIGITAL_WAVEGUIDE
	OSC_REINSCH
	OSC_TYPE_A
	OSC_TYPE_B
	OSC_TYPE_C
	OSC_MAX
)

// 1) digital oscillators have determinant of 1 (they have unity gain)
// 2) digital oscillators have complex eigenvalues

// this makes sense as we want the oscillator to behave like a rotation
// matrix, and rotation matrix exhibit the above properties
func genosc(typ int, t float64) (m [2][2]float64) {
	switch typ {
	case OSC_COUPLED_QUADRATURE:
		k := math.Sin(t)
		s := math.Sqrt(1 - k*k)
		m = [2][2]float64{
			{s, k},
			{-k, s},
		}

	case OSC_MAGIC_CIRCLE:
		k := math.Sin(t / 2)
		m = [2][2]float64{
			{1 - k*k, k},
			{-k, 1},
		}

	case OSC_QUADRATURE_STAGGERED:
		k := math.Cos(t)
		m = [2][2]float64{
			{k, 1 - k*k},
			{-1, k},
		}

	case OSC_BIQUAD:
		k := 2 * math.Cos(t)
		m = [2][2]float64{
			{k, -1},
			{1, 0},
		}

	case OSC_DIGITAL_WAVEGUIDE:
		k := math.Cos(t)
		m = [2][2]float64{
			{k, k - 1},
			{k + 1, k},
		}

	case OSC_REINSCH:
		k := 2*math.Cos(t) - 1
		m = [2][2]float64{
			{k, 1},
			{k - 1, 1},
		}

	case OSC_TYPE_A:
		s := math.Sin(t / 2)
		k := 4 * s * s
		m = [2][2]float64{
			{1 - k, -k},
			{1, 1},
		}

	case OSC_TYPE_B:
		k := 2 * math.Cos(t)
		m = [2][2]float64{
			{0, 1},
			{-1, k},
		}

	case OSC_TYPE_C:
		c := math.Cos(t / 2)
		k := 4 * c * c
		m = [2][2]float64{
			{k - 1, k},
			{-1, -1},
		}
	}
	return
}
