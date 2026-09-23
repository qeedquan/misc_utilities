// http://paulbourke.net/miscellaneous/solver/
// https://en.wikipedia.org/wiki/List_of_Runge%E2%80%93Kutta_methods
package main

import (
	"fmt"
	"math"
)

func main() {
	sim(0, 150, 0.01, 1, exp, expd)
}

func sim(t0, t1, dt, y0 float64, g, f func(float64) float64) {
	var y [7]float64

	for i := range y {
		y[i] = y0
	}

	for t := t0; t < t1; t += dt {
		for i := range y {
			y[i] = int1d(i, dt, y[i], f)
		}
		fmt.Printf("%.6f ", g(t))
		for i := range y {
			fmt.Printf("%.6f ", y[i])
		}
		fmt.Printf("\n")
	}
}

func exp(x float64) float64 {
	return math.Exp(-0.05 * x)
}

func expd(x float64) float64 {
	return -0.05 * x
}

func int1d(m int, h, y0 float64, f func(float64) float64) float64 {
	y := 0.0
	switch m {
	case 0: // euler method
		k1 := h * f(y0)
		y = y0 + k1
	case 1: // modified euler (also known as heuns or explicit trapezoid)
		k1 := h * f(y0)
		k2 := h * f(y0+k1)
		y = y0 + (k1+k2)/2
	case 2: // ralston method
		k1 := h * f(y0)
		k2 := h * f(y0+2*k1/3)
		y = y0 + k1/4 + 3*k2/4
	case 3: // midpoint
		k1 := h * f(y0)
		k2 := h * f(y0+k1/2)
		y = y0 + k2
	case 4: // 3rd order runge-kutta
		k1 := h * f(y0)
		k2 := h * f(y0+k1/2)
		k3 := h * f(y0-k1+2*k2)
		y = y0 + k1/6 + 2*k2/3 + k3/6
	case 5: // 4th order runge-kutta
		k1 := h * f(y0)
		k2 := h * f(y0+k1/2)
		k3 := h * f(y0+k2/2)
		k4 := h * f(y0+k3)
		y = y0 + k1/6 + k2/3 + k3/3 + k4/6
	case 6: // england 4th order, six stage
		k1 := h * f(y0)
		k2 := h * f(y0+k1/2)
		k3 := h * f(y0+(k1+k2)/4)
		k4 := h * f(y0-k2+2*k3)
		y = y0 + k1/6 + 4*k3/6 + k4/6
	}
	return y
}
