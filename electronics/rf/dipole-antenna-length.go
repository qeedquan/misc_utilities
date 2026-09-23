/*

https://www.everythingrf.com/rf-calculators/dipole-antenna-length-calculator
https://www.eham.net/article/23802
https://learn.adafruit.com/adafruit-feather-32u4-radio-with-rfm69hcw-module/antenna-options
https://forums.qrz.com/index.php?threads/best-or-acceptable-wire-size-for-dipole.170575/

Dipole antennas are antennas with two matched conductive elements (solid core wire/metal rods)
This calculates the length of the length of the conductive elements that we need to get to a frequency we want.

We can use one element as an antenna (length/2) or both elements.
Using one element is known as "quarter wave whip antenna"
Using two element is known as "half-wave dipole antenna"

If wires are used, 14-16 AWG are the standard (NEC specifies 14)

*/

package main

import "fmt"

func main() {
	test_freqs := []float64{433, 868, 915, 4000}
	test_160_meter_freqs := []float64{1.8, 1.9, 2.0}
	test_80_meter_freqs := []float64{3.5, 3.6, 3.7, 3.8, 3.9, 4.0}
	test_60_meter_freqs := []float64{5.331, 5.347, 5.367, 5.371, 5.404}
	test_40_meter_freqs := []float64{7, 7.1, 7.15, 7.2, 7.3}
	test_20_meter_freqs := []float64{14, 14.1, 14.2, 14.22, 14.3, 14.313}
	test_10_meter_freqs := []float64{28, 28.5, 29, 29.5}

	chart("Test", test_freqs)

	// the amateur band designation is approximately the length of the two element in meters * 2
	chart("160 meters", test_160_meter_freqs)
	chart("80 meters", test_80_meter_freqs)
	chart("60 meters", test_60_meter_freqs)
	chart("40 meters", test_40_meter_freqs)
	chart("20 meters", test_20_meter_freqs)
	chart("10 meters", test_10_meter_freqs)
}

func chart(band string, freqs []float64) {
	fmt.Printf("%s band\n", band)
	for _, f := range freqs {
		l := length(f)
		e := l / 2

		li := feet2inches(l)
		ei := feet2inches(e)

		lm := feet2meter(l)
		em := feet2meter(e)

		fmt.Printf("%.6f mhz: meters %.6f %.6f | feet %.6f %.6f | inches %.6f %.6f\n", f, lm, em, l, e, li, ei)
	}
	fmt.Println()
}

/*

f in mhz
l in feet

*/
func length(f float64) (l float64) {
	return 468 / f
}

func feet2inches(f float64) float64 {
	return f * 12
}

func feet2meter(f float64) (m float64) {
	return f / 3.281
}
