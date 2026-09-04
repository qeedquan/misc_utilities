// https://www.woolseyworkshop.com/2018/05/05/calculating-the-minimum-resistor-value-for-an-led/

package main

import "fmt"

func main() {
	fmt.Println(ledr(5.5, 1.2, 0.015, 0.1))
	fmt.Println(ledr(5.5, 1.7, 0.018, 0.1))
	fmt.Println(ledr(5.5, 3.0, 0.07, 0.1))
}

// The model here is based on a series connection of an LED with a resistor
// We want to calculate the resistor value for the LED, we can use Ohm's law
// for this
// V=IR, but we do need to figure out what V and I should be
// Vs = supply voltage, how much voltage we feed into the circuit - 5 +- 0.5V
// Vf = LED forward voltage, since an LED is a form of diode, the forward voltage is the amount of minimum voltage needed to get across a diode (1.2-4.0V)
// If = LED forward current, the maximum recommended is anywhere from 15mA to 80mA
// Tr = Resistor tolerance, the resistor fluctations 1-10%
// So what we want is to maximize the resistor value such that it can handle all cases in this model range
// we want to maximize voltage and minimize current

// Maximizing the voltage means we supply the best we can
// so it will be around 5.5 V, the LED forward voltage should be as small as possible, so assume 1.2V
// For the current, we want to minimize it so we use 15 mA multiply by the largest tolerance which is 10%
// this will give a resistor value, then we can round it up to a standard resistor value for simplicity, or else
// we will have to chain resistors together to achieve more precision
func ledr(Vs_max, Vf_min, If_min, Tr_max float64) float64 {
	return (Vs_max - Vf_min) / (If_min * (1 - Tr_max))
}
