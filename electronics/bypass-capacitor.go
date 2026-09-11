/*

http://www.learningaboutelectronics.com/Articles/Bypass-capacitor-calculator.php
http://www.learningaboutelectronics.com/Articles/What-is-a-bypass-capacitor.html
https://www.youtube.com/watch?v=1xicZF9glH0
https://www.youtube.com/watch?v=BcJ6UdDx1vg
https://www.youtube.com/watch?v=ARwBwHZESOY
https://resources.altium.com/p/interplane-capacitance-and-pcb-stackups
https://www.signalintegrityjournal.com/articles/1589-the-myth-of-three-capacitor-values

Bypass capacitors are not strictly necessary but they are a cheap component that can help with noise so it's better to just have it.
Some videos show examples of removing all the bypass capacitor on the circuit board and the noise didn't change in the oscilloscope.
But there are other videos that show that they do help, it really depends on the circuits used on the boards.

Want to put them near components for higher frequencies since at higher frequencies it creates a loop area back to itself since it has low impedance.
Lower frequencies don't matter as much.

People generally just put the capacitors near power supply without any mathematical analysis.
Or they use three different capacitors per power pin for decoupling (10, 1, 0.1 uF).
They just followed what legacy design guidelines recommends (which can be different with modern designs).

*/

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
