package main

import "fmt"

func main() {
	fmt.Println(calcdiv(9, 500*1e-3, 297, 8, 0.58))
}

// Given an AC source passing through a transistor, we want to shift the AC wave up so that there is no clipping/distortion
// to do this shift (dc bias), the common way is to place two resistor in series R1 and R2 before the base transistor
// this is a way to calculate R1 and R2
// https://www.petervis.com/GCSE_Design_and_Technology_Electronic_Products/Potential_Divider/Potential_Divider_Bias.html
// https://uk.farnell.com/q-point-definition
func calcdiv(Vcc, Ic_max, hFE, Rc, Vbe float64) (R1, R2 float64) {
	// Rc represents the load resistance

	// (Vce, Ic) represents the Q point of the transistor in this calculation
	// for max output performance, amplifiers are usually midpoint biased, so
	// we just need to divide by 2 to get the Q point.
	// The Q point represents the steady state DC voltage at a specified terminal
	// of an active device where no input signal is applied
	// Stated another way, biasing in a transistor allows it to operate within a specified region of
	// its transconductance curve. Distortion is avoided, as neither the positive nor negative
	// output swing drives the transistor into a nonlinear operating region.
	// It should be noted that a Q-point does not necessarily remain stable after it has been set up.
	// If a transistor’s junction temperature is allowed to increase, thermal runaway may occur.
	// This will shift the Q-point into the saturation region, causing signal distortion and possible damage to the transistor.
	Ic := Ic_max / 2
	Vce := Vcc / 2
	Ib := Ic / hFE

	// To ensure a good stability factor, the current "I" flowing through the resistors should be in the magnitude of 10
	// times the base current flow as following.
	I := 10 * Ib

	// Since we know the voltage across the voltage divider network to be Vcc,
	// and we have a value of the current "I" flowing through the resistors, we may calculate the sum of R1 + R2
	// (R1 + R2) = Vcc / I
	Rt := Vcc / I

	// Find Re
	// All the voltages across the resistors and transistor add up to give the following expression.
	// Vcc = Ve + Vc + Vce
	// Ve = Ie * Re
	// Vc = Ic * Rc
	// However, Ic and Ie are very similar, with a negligible difference,
	// hence Ie can be removed from the equation and replaced by Ic to give the following expression.
	// Vcc = (Ic * Re) + (Ic * Rc) + Vce
	Re := ((Vcc - Vce) / Ic) - Rc

	// V2 = Ve + Vbe
	// Ve = Ic * Re
	Ve := Ic * Re
	V2 := Ve + Vbe

	// R2 = V2 / I
	R2 = V2 / I
	R1 = Rt - R2
	return
}
