// https://electronicsclub.info/impedance.htm
// http://www.sengpielaudio.com/calculator-InputOutputImpedance.htm
// impedance is the generalization of resistance for AC circuits
// where it varies based on the input frequency where as resistors do not

package main

import (
	"fmt"
	"math"
)

func main() {
	// as frequency increases, the capacitive reactance goes downs
	// a pure capacitor with a dc source acts like a open circuit
	fmt.Println(reactance(50, 1e-6, 0))
	fmt.Println(reactance(10000, 1e-6, 0))

	// as frequency increases, the inductive reactance goes up
	// a pure inductor with a dc source acts like a short circuit
	fmt.Println(reactance(50, 0, 1e-3))
	fmt.Println(reactance(10000, 0, 1e-3))

	// due to the such characteristic
	// a capacitor pass AC but block DC (AC coupled)
	// a inductor pass DC but block high frequency AC
}

// total reactance is the difference between capacitive and inductive
// X = Xl - Xc
// Xc = 1 / (2pifC)
// Xl = 2pifL
func reactance(f, C, L float64) (float64, float64, float64) {
	Xc := 1 / (2 * math.Pi * f * C)
	Xl := 2 * math.Pi * f * L
	return Xc, Xl, Xl - Xc
}

// Zin is the impedance seen at the input to a circuit/device
// it is the combined effect of all resistance, capacitance, inductance
// Zin = Zr + Zl + Zc
// normal to use term input impedance even when only resistance is in play
// input impedance should be high, at least 10 times as the output impedance
// as a rule of thumb, to ensure input signal does overload the source
// of the signal and reduce the signal by a substantial amount

// Zout is the combined effect of all voltage source, resistance, capacitance,
// or inductance to the output of the device, note that Vsource is an equivalent resistance and not the same as supply voltage Vs
// output impedance should be low, less than tenth of load impedance connected to the output, if the impedance is too high, too much signal will be lost
// Zout << Zload (low impedance output) (little voltage is lost driving output, usually best arrangement)
// Zout = Zload (matched impedance) (this arrangement delivers maximum power to the load)
// Zout >> Zload (high impedance output)
