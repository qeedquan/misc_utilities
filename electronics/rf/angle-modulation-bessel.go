/*

https://www.rfcafe.com/references/electrical/bessel.htm
http://ecelabs.njit.edu/ece489/misc/analysis_fm_spectrum.pdf
https://en.wikipedia.org/wiki/Frequency_modulation#Modulation_index

*/

package main

import (
	"fmt"
	"math"
)

func main() {
	test_series_expansion()
	test_single_wave_spectrum()
}

/*

For a carrier modulated by a single cos/sin wave, the frequency spectrum can be determined directly by just evaluating the bessel function of the first order.

The parameters to the bessel function is the modulation index and the sideband number.
The returned value represent the amplitude in that sideband number in the frequency domain for that modulation index.

For particular values of the modulation index, the carrier amplitude becomes zero and all the signal power is in the sidebands.
The carrier represents sideband number 0
Modulation index of 0 means no modulation of the carrier is performed.

modulation_index = peak_frequency_deviation / highest_frequency_component_present

*/

func test_single_wave_spectrum() {
	modulation_index := []float64{0, 0.25, 0.5, 1, 1.5, 2, 2.41, 2.5, 3.0, 4, 5, 5.53, 6, 7, 8, 8.65, 9, 10, 12}
	for i, m := range modulation_index {
		fmt.Printf("% 6.2f | ", m)
		for c := 0; c <= i; c++ {
			fmt.Printf("% .2f ", math.Jn(c, m))
		}
		fmt.Println()
	}
}

/*

FM/PM modulation takes the form of functions that are

E*cos(w*t + B*sin(mu*t))

where B is known as the deviation

For PM modulation
B = delta_peak (the peak deviation)

For FM modulation
B = delta_phi / mu

These are generally known as angle modulated signals

The function above can be written out as a linear combination of cos/sin functions with
bessel functions of the first kind as evaluated at B as the coefficients

E*Sum{-Inf, Inf} J(B) * cos([w_0 + n*mu]*t)

So this says that the bessel coefficients determine the strength of a frequency in the frequency domain

In FM, the carrier and sideband frequencies disappear when the modulation index (β) is equal to a zero crossing of the function for the nth sideband.
For example, the carrier (0th sideband) disappears when the Jn(0,β) plot equals zero.

Adjusting the modulation index to the proper value causes all of the output power to be concentrated in the usable signal, thus increasing efficiency.
The 1st sideband disappears when the Jn(1,β) plot equals zero, the 2nd sideband disappears when the Jn(2,β) equals zero, etc., etc.

*/

func test_series_expansion() {
	ftab := []func(float64, float64) float64{
		f_cossin, f_sinsin, f_coscos, f_sincos,
	}
	ptab := []func(int, float64, float64) float64{
		p_cossin, p_sinsin, p_coscos, p_sincos,
	}

	for i := range ftab {
		const step = 1e-2
		const eps = 1e-8
		const terms = 20

		for B := 0.0; B <= 2*math.Pi; B += step {
			for phi := 0.0; phi <= 2*math.Pi; phi += step {
				x := ftab[i](B, phi)
				y := ptab[i](terms, B, phi)
				assert(math.Abs(x-y) < eps)
			}
		}
	}
}

func assert(x bool) {
	if !x {
		panic("assertion failed")
	}
}

func f_cossin(B, phi float64) float64 {
	return math.Cos(B * math.Sin(phi))
}

func f_sinsin(B, phi float64) float64 {
	return math.Sin(B * math.Sin(phi))
}

func f_coscos(B, phi float64) float64 {
	return math.Cos(B * math.Cos(phi))
}

func f_sincos(B, phi float64) float64 {
	return math.Sin(B * math.Cos(phi))
}

func p_cossin(N int, B, phi float64) float64 {
	s := 0.0
	for n := 2; n <= N; n += 2 {
		s += math.Jn(n, B) * math.Cos(float64(n)*phi)
	}
	s *= 2
	s += math.Jn(0, B)
	return s
}

func p_sinsin(N int, B, phi float64) float64 {
	s := 0.0
	for n := 1; n <= N; n += 2 {
		s += math.Jn(n, B) * math.Sin(float64(n)*phi)
	}
	s *= 2
	return s
}

func p_coscos(N int, B, phi float64) float64 {
	s := 0.0
	p := 1.0
	for n := 2; n <= N; n += 2 {
		s += p * math.Jn(n, B) * math.Cos(float64(n)*phi)
		p = -p
	}
	s = math.Jn(0, B) - 2*s
	return s
}

func p_sincos(N int, B, phi float64) float64 {
	s := 0.0
	p := 1.0
	for n := 1; n <= N; n += 2 {
		s += p * math.Jn(n, B) * math.Cos(float64(n)*phi)
		p = -p
	}
	s *= 2
	return s
}
