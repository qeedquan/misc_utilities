package main

import (
	"fmt"
	"math/rand"
)

func main() {
	testvoltdiv()
	testparallel()
	testtemp()
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

/*

Resistor tolerances don't tend to change across the lifetime of a resistor, but the resistance does change over temperature

The formula is:
Rt = Ro (1 + alpha*delta_temperature)

If we are given parts per million, the formula becomes
Rt = Ro*PPM*delta_temperature

For example, if we have
1000 Ohm resistor with a 100 PPM experiencing a +/- 60 Kelvin temperature change
1000 * 100*10^-6 * 60 = 6 Ohm

The lower the temperature coefficient the more expensive the resistor will be, this is the same for low tolerance resistors also

Power dissipation can generate heat and therefore change the resistance as the circuit is running, this is known as resistor self-heating
For precision circuits, this can be become a factor
This is usually specified as Celcius/Watt in data sheets

For circuits where it is important that the nominal value is maintained, try to use components that has the close to the same tolerances and temperature coefficients as possible

*/

func testtemp() {
	Ro := 1000.0
	ppm := 100 * 1e-6
	dT := 60.0
	fmt.Println(Ro * ppm * dT)
}

func randv(s float64, n int) []float64 {
	v := make([]float64, n)
	for i := range v {
		v[i] = rand.Float64() * s
	}
	return v
}

/*

In general, circuits cannot be incrementally computed, alot of circuit elements are coupled in the sense that we need to know all of them in the system in advance to calculate the voltage/current distributed between them.
We can't just add one element at a time and use the old computed voltage/current to calculate the new ones.

We can show this by just showing if one circuit element has this property, then circuits in general can't have this property.
Use resistor to show this:

Series resistance is calculated as:
I = V/(R1 + R2 + ... Rn)
V1 = I*R1
V2 = I*R2
etc

If we plugin I we see the following:
V1 = V * R1/(R1 + R2 ... Rn)
V2 = V * R2/(R1 + R2 ... Rn)

So what this equation is showing is that we are computing how much percentage a resistor is part of the whole (R1 + R2 ... Rn)
We can't incrementally compute the value of the next Rn from the previous values of R1, R2, ..., it has to be given to us from the start.

The way this is dealt with in practice if we want to treat circuit elements like lego block is to enforce a restrictions on the circuit elements used:

The circuit elements need to satisfy:
The input impedance is high
The output impedance is low

This restriction allows us to add new elements like lego blocks, even if the impedance change with the new elements, it won't change the old voltage/current enough
to invalidate the old calculations (it will contribute too small of a value to be of significance to the entire circuit)

*/

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
