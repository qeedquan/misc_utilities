// http://hyperphysics.phy-astr.gsu.edu/hbase/Tables/wirega.html
// http://hyperphysics.phy-astr.gsu.edu/hbase/electric/resis.html#c2
package main

import (
	"fmt"
	"math"
	"os"
	"text/tabwriter"
)

func main() {
	w := tabwriter.NewWriter(os.Stdout, 0, 0, 1, ' ', 0)
	defer w.Flush()

	fmt.Fprintf(w, "AWG\tDiameter (in)\tDiameter (mm)\tTurns of Wire (no insulation) (per in)\tTurns of Wire (no Insulation) (per cm)\tArea kcmil\tArea mm^2\tResistance per 1kft (ohms)\tResistance per 1km (ohms)\n")
	for n := -3.0; n <= 40; n++ {
		in := awg2d(n)
		mm := in2mm(in)
		mm2 := d2a(mm)
		kcmil := mm2cmil(mm2) / 1e3

		// to get number of turns of wire, take the inverse of the diameter of the gauge
		// this effectively gives the upper bound on the helix shape one would make if we lay out
		// the wire like this and the helix corresponds to the turns of wire
		twin := 1 / in
		twcm := 10 / mm

		p := resitivity("copper_annealed")
		l := 1e3
		A := d2a(mm / 1e3)
		rko := p * l / A

		// if we want to convert per meter to per foot
		// treat the meter as a foot and then just use
		// the feet to meter formula
		rfo := ft2m(rko)

		fmt.Fprintf(w, "%.0f\t%6f\t%6f\t%6f\t%6f\t%6f\t%6f\t%8f\t%8f\n", n, in, mm, twin, twcm, kcmil, mm2, rfo, rko)
	}
}

// diameter to area
func d2a(d float64) float64 {
	return math.Pi * d * d / 4
}

// awg to diameter
func awg2d(n float64) float64 {
	return 0.005 * math.Pow(92, (36-n)/39)
}

// inch to millimeter
func in2mm(in float64) float64 {
	return in * 25.4
}

// squared millimeter to circular mil
func mm2cmil(mm2 float64) float64 {
	r := 0.5 * 0.0254
	return mm2 / (math.Pi * r * r)
}

func resitivity(m string) float64 {
	switch m {
	case "silver":
		return 1.59 * 1e-8
	case "copper":
		return 1.68 * 1e-8
	case "copper_annealed":
		return 1.724 * 1e-8
	}
	return 0
}

// feet to meter
func ft2m(ft float64) float64 {
	return ft * 0.3048
}
