package main

import (
	"fmt"
	"math"
	"math/cmplx"

	"github.com/qeedquan/go-media/fftw"
)

func main() {
	hz := 1234
	in := fftw.AllocReal(nextpow2(2*hz + 1))
	out := fftw.AllocComplex(len(in))

	plan := fftw.PlanDFT_R2C_1D(len(in), in, out, 0)
	for i := range in {
		for h := 0; h <= 10; h++ {
			w := 2 * math.Pi * float64(hz-h*12)
			t := float64(i) / float64(len(in))
			in[i] += math.Sin(w * t)
		}
	}

	fftw.Execute(plan)
	fftw.DestroyPlan(plan)
	for i := range out {
		if cmplx.Abs(out[i]) > 1e-6 {
			fmt.Println(i, cmplx.Abs(out[i]))
		}
	}
	fftw.Free(in)
	fftw.Free(out)
}

func nextpow2(n int) int {
	p := 1
	for p < n {
		p <<= 1
	}
	return p
}
