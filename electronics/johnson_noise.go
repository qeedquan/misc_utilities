// http://www.sengpielaudio.com/calculator-noise.htm
// https://www.physics.utoronto.ca/~phy224_324/experiments/thermal-noise/Thermal-Noise.pdf
package main

import (
	"fmt"
	"math"
)

func main() {
	V := JN(20, 20, 20000, 200)
	fmt.Println(V, LU(V), LV(V))
}

// johnson noise
// Vn = sqrt(4 * k_b * T * df * R)
// Vn voltage noise (uV)
// k_b boltzmann constant
// T temperature (C, formula uses kelvin so need conversion)
// df (fmax - fmin) (Hz)
// R resistance (ohms)
func JN(T, fmin, fmax, R float64) float64 {
	const k_b = 1.38064852e-23
	return math.Sqrt(4 * k_b * (273.15 + T) * (fmax - fmin) * R)
}

// noise voltage level (audio)
// Lu = 20 * log10(V/V0) dBu
// V0 = 774596.67 uV
func LU(V float64) float64 {
	const V0 = 0.7746
	return 20 * math.Log10(V/V0)
}

func LV(V float64) float64 {
	const V0 = 1.0
	return 20 * math.Log10(V/V0)
}
