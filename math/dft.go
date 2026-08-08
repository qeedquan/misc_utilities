package main

import (
	"fmt"
	"math"
	"math/cmplx"
	"math/rand"
	"time"
)

func main() {
	rand.Seed(time.Now().UnixNano())
	testbasic()
	testmul()
	testscale()
	testpad()
	testdecomp1()
	testdecomp2()
	testdecomp3()
	testsample1()
	testsample2()
	testphase()
	testresponse()
}

// a fir filter frequency magnitude response
// can be calculated by taking the dft of the fir coefficients (the impulse response)
// and also its phase, the magnitude and phase output in the dft domain tells us
// how convolving the input signal against the filter will behave (how large is the peak, the phase shift)
// at various frequency
func testresponse() {
	// test an average filter 1/5*(5 samples)
	// with a 64 point dft, that means we can get the 64/2 = 32
	// magnitude response and phase shift, it means we also need to step by 2
	// ie, if we sample a signal at Fs/32 the magnitude and response will be at f[2]
	// if we do it at 3Fs/32, the answer will be at f[6]
	a := [64]float64{.2, .2, .2, .2, .2}
	b := dft1(float2complex(a[:]))
	for i := range b {
		fmt.Println(i, cmplx.Abs(b[i]), rad2deg(cmplx.Phase(b[i])))
	}
}

func testbasic() {
	x := []complex128{
		complex(1, 0),
		complex(2, -1),
		complex(0, -1),
		complex(-1, 2),
	}
	y := dft1(x)
	z := idft1(y)
	fmt.Printf("%.2v\n", x)
	fmt.Printf("%.2v\n", y)
	fmt.Printf("%.2v\n", z)

}

// test decomposition of signal into frequency domain
func testdecomp1() {
	// for periodic signal made up of sines/cosines
	// dft can detect both of them, it stores
	// cosine in real term and sine in imaginary term
	// the absolute value can be used to detect if
	// this frequency is used or not
	// it mirrors the values over N/2 range

	// if there is a dc offset, frequency 0 in the frequency domain
	// will be shifted proportional to magnitude of offset
	f2 := func(t float64) float64 {
		return f1(t) + 10
	}
	for n := 1; n <= 32; n++ {
		a := sample(f1, 0, 1, 1000)
		A := sample(f2, 0, 1, 1000)
		for i := range a {
			a[i] *= complex(float64(n), 0)
			A[i] *= complex(float64(n), 0)
		}
		b := dft1(a)
		B := dft1(A)
		for i := range b {
			if cmplx.Abs(B[i]) > 1e-3 {
				fmt.Println(i, cmplx.Abs(b[i]), b[i], cmplx.Abs(B[i]), B[i])
			}
		}
		fmt.Println()
	}
}

func testdecomp2() {
	// sampling square wave over 2pi interval
	// gives all the harmonics (odd)
	a := sample(f2, 0, 2*math.Pi, 4096)
	b := dft1(a)
	for i := range b {
		if cmplx.Abs(b[i]) > 1 {
			fmt.Println(i, cmplx.Abs(b[i]), b[i])
		}
	}
}

func testdecomp3() {
	a := sample(f3, 0, 1, 1<<13)
	b := dft1(a)
	for i := range b {
		if cmplx.Abs(b[i]) > 1e-3 {
			fmt.Println(i, 2*cmplx.Abs(b[i])/float64(len(b)))
		}
	}
}

// sampling at a wave of (f0 + kfs) more than the original
// wave of f0 frequency gives same values, meaning we cannot distinguish
// between waves of f0 and (f0 + kfs) frequency if we alias
func testsample1() {
	f0 := 41.0
	sr := int(f0) * 71

	F := func(t float64) float64 {
		return math.Sin(2 * math.Pi * f0 * t)
	}
	G := func(t float64) float64 {
		return math.Sin((f0 + 20*float64(sr)) * 2 * math.Pi * t)
	}
	a := sample(F, 0, 1, sr)
	b := sample(G, 0, 1, sr)
	for i := range a {
		fmt.Println(a[i], b[i])
	}
}

// dft output size does not have to be the same size as input size
// if the dft output size is smaller, then the points in the frequency
// domain are multiples of a fundamental frequency 1F, 2F, etc where
// F = sample_rate/output_size, ie, if we sample 8000 samples
// and we use a 8 point DFT, each point in the DFT output represents
// frequencies multiple of 1000 (since 8000/8 = 1000)
// dft is symmetric	around n/2 for real signals, and for complex signals
// the higher n/2 indices stores the complex conjugate of the lower portion
// (ie, m[1] = m[n-1], m[2] = m[n-2] ...
// for odd function m_real is 0 while m_imag is generally not 0
// for even function m_real is generally not 0 while m_imag is 0
// for magnitude of dft, for real signals magnitude is A_0*N/2
// where A_0 is the largest amplitude with integral number of cycles over N cycles
// so here we the amplitude of the DFT is 4000, because 1*(8000/2) = 4000 where A_0 = 1
// and N = 8000; for complex signals it is A_0*N
func testsample2() {
	f := func(t float64) float64 {
		n := 1.0
		return math.Sin(2*math.Pi*1000*n*t) + 0.5*math.Sin(2*math.Pi*2000*n*t+3*math.Pi/4)
	}
	x := sample(f, 0, 1, 8000)
	y := dft1n(x, 8)
	for i := range y {
		fmt.Println(i, cmplx.Abs(y[i]))
	}
}

// delay sampling preserves the magnitude of the DFT
// but the phase is shifted by 2*pi*k*m/N
func testphase() {
	N := 8
	f := func(t float64) float64 {
		return math.Sin(2*math.Pi*1000*t) + 0.5*math.Sin(2*math.Pi*2000*t+3*math.Pi/4)
	}
	for i := 0; i < 10; i++ {
		x := sample(f, 0, 1, 8000)
		y := dft1n(x[:N], N)
		z := dft1n(x[i:i+N], N)
		for i := 0; i < N; i++ {
			fmt.Println(i, cmplx.Abs(y[i]), cmplx.Abs(z[i]), rad2deg(cmplx.Phase(y[i])), rad2deg(cmplx.Phase(z[i])))
		}
		fmt.Println()
	}
}

func f1(t float64) float64 {
	s := 0.0
	for i := 1; i <= 5; i++ {
		w := 10 * 2 * math.Pi
		// multiplying by a constant scales
		// the values in frequency domain
		// ie, 3*cos means 3*v where v is
		// (real) value in frequency domain
		// and 5*sin means 5*v (imaginary) in frequency domain
		s += float64(i) * (3*math.Cos(float64(i)*w*t) + 5*math.Sin(float64(i)*w*t))
	}
	return s
}

// square wave
func f2(t float64) float64 {
	s := 0.0
	for i := 1; i < 1024; i += 2 {
		s += math.Sin(float64(i)*t) / float64(i)
	}
	return s
}

// various amplitudes and harmonics
// doing an fft on this will give the amplitude at each
// frequency coordinates, ie
// f[0] = amplitude of sin(wt)
// f[1] = amplitude of sin(2wt)
// f[7] = amplitude of cos(7wt)
// f[33] = amplitude of cos(33wt) and so on
// however if we mix sin/cos as in sin*cos*sin
// the frequency amplitude will get spread out (equally) over multiple frequency channels
// spreading occurs in with the center (the highest frequency component in the product is the center location)
// and it spreads out to the sum of all the other frequency components, ie 10+8+40=58
// for sin(10wt)*sin(8wt)*sin(40wt)
func f3(t float64) float64 {
	f := 1.0
	w := 2 * math.Pi * f
	return math.Sin(w*t) +
		0.4*math.Sin(2*w*t) +
		3.3*math.Cos(7*w*t) +
		5.6*math.Cos(33*w*t) +
		27*math.Sin(10*w*t)*math.Sin(8*w*t)*math.Sin(40*w*t)*math.Sin(150*w*t) +
		81*math.Cos(11*w*t)*math.Cos(12*w*t)
}

func sample(f func(float64) float64, s, e float64, n int) []complex128 {
	var p []complex128

	d := (e - s) / float64(n)
	for i := 0; i < n; i++ {
		p = append(p, complex(f(s+float64(i)*d), 0))
	}
	return p
}

// adding zero padding to a non-power of two
// array gives very different values in frequency domain
func testpad() {
	v := randcmplxv(64, 1)

	s := 32
	for i := s; i < len(v); i++ {
		v[i] = complex(0, 0)
	}
	for i := s; i < len(v); i++ {
		fmt.Printf("%v points\n", i)
		p := dft1(v[:i])
		q := idft1(p)
		for j := 0; j < i; j++ {
			fmt.Println(p[j], q[j])
		}
	}
}

// scaling the values in the time domain
// causes the ratio in scaled frequency domain to
// be of sequence f_n+1/f_n = n/(n+1) as one
// scale 1x, 2x, 3x, 4x ....
func testscale() {
	v := randcmplxv(128, 3)
	p := append([]complex128{}, v...)

	var a, b []complex128
	for i := 1; i < 128; i++ {
		for n := range p {
			p[n] = v[n] * complex(float64(i), 0)
		}
		b = a
		a = dft1(p)
		fmt.Printf("scale by %v with %v points ", i, len(p))
		if b != nil {
			R1 := cmplx.Abs(a[0]) / cmplx.Abs(b[0])
			fmt.Println(R1, 1/R1, float64(i-1)/(float64(i)))
		} else {
			fmt.Println()
		}
	}
}

func testmul() {
	fmt.Println(imul(1000, 2000))
	fmt.Println(imul(546, 243))
	fmt.Println(imul(80581, 9563214))
	fmt.Println(imul(1000000, 20000000000))
	for i := 0; i < 10000; i++ {
		a := rand.Int()
		b := rand.Int()
		if a*b != imul(a, b) {
			fmt.Println(a*b, imul(a, b))
		}
	}
}

func randcmplxv(n int, s float64) []complex128 {
	p := make([]complex128, n)
	for i := range p {
		p[i] = randcmplx(s)
	}
	return p
}

func randcmplx(n float64) complex128 {
	return complex(rand.Float64()*n, rand.Float64()*n)
}

func dft1(x []complex128) []complex128 {
	return dft1n(x, len(x))
}

func dft1n(x []complex128, sz int) []complex128 {
	y := make([]complex128, sz)
	for k := range y {
		for n := range x {
			c := -2 * math.Pi / float64(sz) * float64(k) * float64(n)
			v := complex(0, c)
			y[k] += x[n] * cmplx.Exp(v)
		}
	}
	return y
}

func idft1(x []complex128) []complex128 {
	y := make([]complex128, len(x))
	for n := range x {
		for k := range x {
			c := 2 * math.Pi * float64(k) * float64(n) / float64(len(x))
			v := complex(0, c)
			y[n] += x[k] * cmplx.Exp(v)
		}
		y[n] *= complex(1/float64(len(x)), 0)
	}
	return y
}

// http://www.cs.rug.nl/~ando/pdfs/Ando_Emerencia_multiplying_huge_integers_using_fourier_transforms_paper.pdf
func imul(a, b int) int {
	const B = 10

	x := iseq(a, B)
	y := iseq(b, B)
	l := max(len(x), len(y)) * 2
	x = append(x, make([]complex128, l-len(x))...)
	y = append(y, make([]complex128, l-len(y))...)

	X := dft1(x)
	Y := dft1(y)
	Z := make([]complex128, l)
	for i := range Z {
		Z[i] = X[i] * Y[i]
	}
	z := idft1(Z)

	s := 0
	p := 1
	for i := range z {
		s += int(math.Round(real(z[i]))) * p
		p *= B
	}
	return s
}

func iseq(a, base int) []complex128 {
	var p []complex128
	for i := 0; a != 0; i++ {
		p = append(p, complex(float64(a%base), 0))
		a /= base
	}
	return p
}

func max(a, b int) int {
	if a > b {
		return a
	}
	return b
}

func float2complex(x []float64) []complex128 {
	p := make([]complex128, len(x))
	for i := range p {
		p[i] = complex(x[i], 0)
	}
	return p
}

func rad2deg(x float64) float64 {
	return x * 180 / math.Pi
}
