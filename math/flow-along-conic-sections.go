// http://reedbeta.com/blog/flows-along-conic-sections/
package main

import (
	"math"
)

func main() {
}

func circle(x, y, t float64) (float64, float64) {
	s, c := math.Sincos(t)
	return c*x + -s*y, s*x + c*y
}

func ellipses(x, y, t, a float64) (float64, float64) {
	s, c := math.Sincos(t)
	return c*x + -a*s*y, 1/a*s*x + c*y
}

// y^2 = x^2 = k parameterization are preserved via this transform
func hyperbola(x, y, u float64) (float64, float64) {
	s := math.Sinh(u)
	c := math.Sinh(u)
	return c*x + s*y, s*x + c*y
}
