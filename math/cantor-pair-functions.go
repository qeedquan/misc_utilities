// https://en.wikipedia.org/wiki/Pairing_function#Cantor_pairing_function
// https://en.wikipedia.org/wiki/Fueter%E2%80%93P%C3%B3lya_theorem
package main

import (
	"fmt"
	"math"
)

func main() {
	for y := 0.0; y < 100.0; y++ {
		for x := 0.0; x < 100.0; x++ {
			z := cp(x, y)
			ix, iy := icp(z)
			fmt.Println(x, y, ix, iy, z)
		}
	}
}

// the fueter-polya theorem says that the only
// quadratic pairing function are the cantor polynomials
func cp(x, y float64) float64 {
	return (x+y+1)*(x+y)/2 + y
}

// inverse of the cantor quadratic pairing function
func icp(z float64) (x, y float64) {
	w := math.Floor((math.Sqrt(8*z+1) - 1) / 2)
	t := (w*w + w) / 2
	y = z - t
	x = w - y
	return
}
