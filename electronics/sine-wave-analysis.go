/*

https://ccrma.stanford.edu/~jos/filters/Sine_Wave_Analysis.html
https://www.dsprelated.com/freebooks/filters/Simulated_Sine_Wave_Analysis_Matlab.html

This is basically a manual bode plot

When doing sine-wave analysis, the cosine function is used because it gives a value amplitude response at the zero frequency (so you can think of it as a valid starting point), whereas sine would give NaNs
The amplitude response analysis is a ratio, so if we pass in a scale factor for the amplitude it does not affect amplitude response values (however changing the phase does, it moves the amplitude response)

*/

package main

import (
	"fmt"
	"math"
)

func main() {
	analyze([]float64{1, 1}, 100, 434.56, 0)
	analyze([]float64{1, 1}, 20, 434.56, math.Pi/4)
	analyze([]float64{0.5, 0.3, 0.4, 0.5, 0.3, 0.1, 0.5, 0.7}, 50, 1, 0)
}

func analyze(fircoeffs []float64, samprate, amp, phase float64) {
	fn := samprate / 2
	tn := 1.0
	dt := 1 / samprate
	fmt.Printf("fir=%v nyquist_freq=%v amp=%v phase=%v\n", fircoeffs, fn, amp, phase)
	for f := 0.0; f <= fn; f += 1 {
		ampin_c := -math.MaxFloat64
		ampin_s := -math.MaxFloat64
		for t := 0.0; t <= tn; t += dt {
			val_c := amp * math.Cos(2*math.Pi*t*f+phase)
			val_s := amp * math.Sin(2*math.Pi*t*f+phase)

			ampin_c = max(ampin_c, val_c)
			ampin_s = max(ampin_s, val_s)
		}

		fir_c := newfir(fircoeffs)
		fir_s := newfir(fircoeffs)

		ampout_c := -math.MaxFloat64
		ampout_s := -math.MaxFloat64
		for t := 0.0; t <= tn; t += dt {
			val_c := amp * math.Cos(2*math.Pi*t*f+phase)
			val_s := amp * math.Sin(2*math.Pi*t*f+phase)

			fir_c.Update(val_c)
			fir_s.Update(val_s)

			ampout_c = max(ampout_c, fir_c.Output())
			ampout_s = max(ampout_s, fir_s.Output())
		}

		// we don't handle transients, but for fir filter we should chop off the transient response before looking at the gains to be more accurate
		// the transient response is the length of the filter - 1
		fmt.Printf("%.1f cos_amp [%.3f %.3f %3f] sin_amp [%.3f %.3f %.3f]\n",
			f, ampin_c, ampout_c, ampout_c/ampin_c, ampin_s, ampout_s, ampout_s/ampin_s)
	}
	fmt.Println()
}

type FIR struct {
	b []float64
	x []float64
	n int
	i int
}

func newfir(b []float64) *FIR {
	return &FIR{
		b: b,
		x: make([]float64, len(b)),
		n: len(b),
	}
}

func (f *FIR) Reset() {
	clear(f.x)
	f.i = 0
}

func (f *FIR) Order() int {
	return f.n - 1
}

func (f *FIR) Update(x float64) {
	f.x[f.i] = x
	f.i = (f.i + 1) % f.n
}

func (f *FIR) Output() float64 {
	r := 0.0
	for i := range f.b {
		j := f.i - i - 1
		if j < 0 {
			j = i
		}
		r += f.b[i] * f.x[j]
	}
	return r
}
