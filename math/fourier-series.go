package main

import (
	"fmt"
	"math"

	"github.com/qeedquan/go-media/math/f64"
)

func main() {
	ROC := 30.0
	STEP := 1e-3
	f := f3
	g := series(f, ROC, 1000)
	for i := -ROC; i <= ROC; i += STEP {
		fmt.Println(i, f(i), g(i))
	}
}

// http://tutorial.math.lamar.edu/Classes/DE/FourierSeries.aspx
// finds a fourier series with a region of convergence (ROC)
// the answer could be wildly wrong outside the ROC
func series(f func(float64) float64, ROC float64, n int) func(float64) float64 {
	N := 100000
	L := ROC

	A := make([]float64, n+1)
	B := make([]float64, n+1)
	for i := 0; i <= n; i++ {
		switch i {
		case 0:
			A[i] = 1.0 / (2 * L) * f64.Simpson1D(f, -L, L, N)
		default:
			g := func(x float64) float64 {
				return f(x) * math.Cos(float64(i)*math.Pi*x/L)
			}
			h := func(x float64) float64 {
				return f(x) * math.Sin(float64(i)*math.Pi*x/L)
			}

			A[i] = 1.0 / L * f64.Simpson1D(g, -L, L, N)
			B[i] = 1.0 / L * f64.Simpson1D(h, -L, L, N)
		}
	}

	return func(x float64) float64 {
		s := 0.0
		for i := range A {
			s += A[i] * math.Cos(float64(i)*math.Pi*x/L)
			s += B[i] * math.Sin(float64(i)*math.Pi*x/L)
		}
		return s
	}
}

func f1(x float64) float64 {
	return x*x + x + 1
}

func f2(x float64) float64 {
	return math.Exp(-(x * x))
}

func f3(x float64) float64 {
	return 4*x*x*x + x*1 - x*math.Sin(x)
}
