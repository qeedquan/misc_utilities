package main

import (
	"fmt"
	"math/rand"
	"time"
)

func main() {
	rand.Seed(time.Now().UnixNano())
	testvoltdiv()
	testparallel()
}

func testvoltdiv() {
	// a resistor connected in series with the same resistor
	// values can act as a voltage divider into fractional parts
	// if we connect 2 resistor in series, the voltage will
	// 50-50 on each resistor, if we have 3, then it'd be 33-33-33 and so on
	// the values of the resistor determines the current flowing through it
	// but the effect of the voltage division is the same no matter what
	// value of the resistance is as long as they are in series and the
	// same value

	// a quick shortcut is take
	// Vdiv = Vcc / NumResistorsInSeries to get the voltage at each junction
	// if all the resistor in series is equal
	fmt.Printf("Same resistance in series with constant voltage source gives a voltage divider")
	fmt.Println(series(1.5, []float64{4.7, 4.7, 4.7}))
	fmt.Println(series(1000, []float64{2, 2, 2, 2}))
	fmt.Println(series(17, []float64{4.7, 4.7, 4.7}))
	fmt.Println(series(159, []float64{1, 1, 1, 1, 1, 1, 1}))
	fmt.Println(series(34114, []float64{10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10}))
	fmt.Println()

	// see that it doesn't split it evenly anymore once we have different
	// resistance values
	fmt.Println("Constant voltage source with different resistance #1")
	fmt.Println(series(17, []float64{3, 4, 5}))
	fmt.Println()

	fmt.Println("Constant voltage source with different resistance #2")
	fmt.Println(series(3.3, []float64{20000, 20000, 20000, 200, 10000}))
	fmt.Println()

	// The voltage divider equation for 2 resistance is
	// Vout = Vin*R2 / (R1+R2)
	// Derivation:
	// R1 and R2 are hooked up in series, so the current is constant over these resistors
	// Rt = R1 + R2
	// A = Vin / Rt
	// To calculate Vout, note that Vout is placed after R1, and hence it is considered a junction at R2 so
	// Vout = A * R2, expand out A
	// Vout = (Vin * R2) / Rt
	// Vout = Vin*R2 / (R1 + R2)
}

func testparallel() {
	// for parallel circuits, the voltage is constant while
	// current varies at each resistor

	// a quick shortcut
	// Rt = Vcc / It
	// It = I1 + I2 + I3 ...
	// 1/Rt = 1/R1 + 1/R2 + 1/R3 ...
	fmt.Println("Constant voltage source with parallel resistors calculation")
	for i := 0; i < 10; i++ {
		Vcc := rand.Float64() * 30
		I, Rt := parallel(Vcc, []float64{rand.Float64() * 1000, rand.Float64() * 100, 30, 40})
		It := 0.0
		for i := range I {
			It += I[i]
		}
		fmt.Println(I, Vcc/It, Rt)
	}
	fmt.Println()

	// if we add more resistor in parallel, for the same
	// voltage, we can draw more current, since more parallel paths
	// mean more locations for the charges to move around
	fmt.Println("Constant voltage source with increasing parallel resistors that are random gives more current")
	var R []float64
	Vcc := rand.Float64() * 100
	for i := 0; i < 10; i++ {
		R = append(R, rand.Float64()*100)
		I, _ := parallel(Vcc, R)
		It := 0.0
		for i := range I {
			It += I[i]
		}
		fmt.Println(Vcc, It)
	}
}

func randv(s float64, n int) []float64 {
	v := make([]float64, n)
	for i := range v {
		v[i] = rand.Float64() * s
	}
	return v
}

func series(Vcc float64, R []float64) (V []float64, I, Rt float64) {
	for i := range R {
		Rt += R[i]
	}

	I = Vcc / Rt
	V = make([]float64, len(R))
	for i := range V {
		V[i] = I * R[i]
	}

	return
}

func parallel(Vcc float64, R []float64) (I []float64, Rt float64) {
	I = make([]float64, len(R))
	for i := range R {
		I[i] = Vcc / R[i]
		Rt += 1 / R[i]
	}
	Rt = 1 / Rt
	return
}
