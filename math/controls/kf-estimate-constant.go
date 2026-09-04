// https://www.cs.unc.edu/~tracker/media/pdf/SIGGRAPH2001_CoursePack_08.pdf
// given a constant not known but can be measured with some noise, estimate it using kalman filter

package main

import (
	"fmt"
	"math"
	"math/rand"
)

func main() {
	sim(0.0, rand.Float64()*100, 0.1, 1e-5, 1.0, 0.01, 1000)
	sim(0.0, rand.Float64()*100, 0.1, 0, 0.95, 0.01, 1000)
	sim(-100.0, rand.Float64()*100, 0.1, 0, 0.95, 0.01, 5000)
	sim(13094.0, rand.Float64()*1000, 0.2, 0, 0.95, 0.2, 100000)
	sim(194.0, rand.Float64()*10000, 0.0, 0, 0.95, 0.2, 100000)
}

func sim(x, z, t, Q, P, R float64, N int) {
	x0 := x
	for i := 0; i < N; i++ {
		x, P = update(x, measure(z, t), P, Q, R)
	}
	fmt.Printf("N %d x0 %.6f x %.6f z %.6f err %.6f\n", N, x0, x, z, math.Abs(x-z))
}

func measure(z, t float64) float64 {
	n := 2*rand.Float64() - 1
	return z + n*t*z
}

func update(x, z, P, Q, R float64) (xn, Pn float64) {
	K := P / (P + R)
	xn = x + K*(z-x)
	Pn = (1-K)*P + Q
	return
}
