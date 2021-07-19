// http://www.learningaboutelectronics.com/Articles/Bypass-capacitor-calculator.php
// http://www.learningaboutelectronics.com/Articles/What-is-a-bypass-capacitor.html

package main

import (
	"fmt"
	"math"
)

func main() {
	fmt.Println(bycap(45, 2))
	fmt.Println(bycap(452, 12))
	fmt.Println(byfreq(bycap(452, 12), 12))

	fmt.Println(byfreq(10*1e-6, 1e3))
	fmt.Println(byfreq(1*1e-6, 1e3))
}

// rule of thumb is that resistance that the capacitor offers to the
// AC signal should be 0.1 (1/10th) or less than the resistor in parallel
// with the capacitor, we can use that to calculate the reactance Xc
// and use the cutoff frequency formula to get the value of the capacitor
// cutoff frequency is also known as corner crossover, break, half-power, and 3db frequency
func bycap(f, R float64) float64 {
	Xc := 0.1 * R
	return 1 / (2 * math.Pi * f * Xc)
}

func byfreq(C, R float64) float64 {
	Xc := 0.1 * R
	return 1 / (2 * math.Pi * C * Xc)
}
