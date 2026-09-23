// https://en.wikipedia.org/wiki/Total_harmonic_distortion

package main

import (
	"fmt"
	"math"
	"math/cmplx"

	"github.com/qeedquan/go-media/fftw"
)

func main() {
	fftw.SetTimeLimit(0.5)
	testsquare()
	testsawtooth()
	testtriangle()
}

func testsquare() {
	n1 := 4096
	f1 := func(t float64) float64 { return square(1, t, n1) }
	f2 := func(t float64) float64 { return exactsquare(1, t) }
	in1 := sample(2*n1+1, f1)
	in2 := sample(2*n1+1, f2)
	thd := math.Sqrt(math.Pi*math.Pi/8 - 1)
	fmt.Println(fftthd(in1), fftthd(in2), thd)
}

func testsawtooth() {
	n1 := 32768
	f1 := func(t float64) float64 { return sawtooth(1, t, n1) }
	in := sample(n1, f1)
	thd := math.Sqrt(math.Pi*math.Pi/6 - 1)
	fmt.Println(fftthd(in), thd)
}

func testtriangle() {
	n1 := 32768
	f1 := func(t float64) float64 { return triangle(1, t, n1) }
	in := sample(n1, f1)
	thd := math.Sqrt(math.Pow(math.Pi, 4)/96 - 1)
	fmt.Println(fftthd(in), thd)
}

func sample(n int, f func(float64) float64) []float64 {
	p := make([]float64, n)
	for i := 0; i < n; i++ {
		p[i] = f(float64(i) / float64(n))
	}
	return p
}

// idea is to take the fft to get the magnitude content for each frequency
// sum them up rms wise and divide by the first fundamental to get the total harmonic distortion
func fftthd(val []float64) float64 {
	in := fftw.AllocReal(len(val))
	defer fftw.Free(in)

	out := fftw.AllocComplex(len(in))
	defer fftw.Free(out)

	plan := fftw.PlanDFT_R2C_1D(len(in), in, out, 0)
	defer fftw.DestroyPlan(plan)
	copy(in, val)

	fftw.Execute(plan)
	return thd(out)
}

func exactsquare(A, t float64) float64 {
	if t < 0.5 {
		return A
	}
	return 0
}

func square(A, t float64, n int) float64 {
	a := A * 4 / math.Pi
	v := 0.0
	for i := 0; i < n; i++ {
		f := 2*float64(i+1) - 1
		v += math.Sin(2*math.Pi*f*t) / f
	}
	return v * a
}

func sawtooth(A, t float64, n int) float64 {
	v := 0.0
	s := -1.0
	for i := 0; i < n; i++ {
		f := float64(i) + 1
		v += s * math.Sin(2*math.Pi*f*t) / f
		s = -s
	}

	return A/2 - A/math.Pi*v
}

func triangle(A, t float64, n int) float64 {
	v := 0.0
	s := 1.0
	for i := 0; i < n; i++ {
		m := 2*float64(i) + 1
		v += s * (1 / (m * m)) * math.Sin(2*math.Pi*m*t)
		s = -s
	}
	return A * 8 / (math.Pi * math.Pi) * v
}

func thd(v []complex128) float64 {
	if len(v) < 2 {
		return 0
	}

	vh := 0.0
	for i := 2; i < len(v); i++ {
		x := cmplx.Abs(v[i])
		vh += x * x
	}
	return math.Sqrt(vh) / cmplx.Abs(v[1])
}
