// https://web.stanford.edu/class/ee133/handouts/labs2003/prelab1mult.pdf
// https://pages.uoregon.edu/soper/Light/frequency.html
package main

import (
	"flag"
	"fmt"
	"os"
	"strconv"
)

func main() {
	flag.Parse()
	if flag.NArg() < 1 {
		test()
	}

	for _, arg := range flag.Args() {
		hz, err := strconv.ParseFloat(arg, 64)
		if err != nil {
			fmt.Fprintf(os.Stderr, "%v: %v\n", arg, err)
			continue
		}
		fmt.Printf("%f hz: %f m\n", hz, wavelength(hz))
	}
}

func test() {
	// to maximize efficiency of transmission, physical dimensions
	// of transmitting and receiving antennas must be on the same order of
	// magnitude for the wavelength of the signal to be transmitted

	// since audio is quite low frequency, the wavelength is huge
	// which makes antenna not feasible, which is why modulation is needed
	fmt.Println("audio frequency")
	audio := []float64{20, 20 * 1e3}
	for _, v := range audio {
		fmt.Printf("%f hz: %f km\n", v, wavelength(v)/1e3)
	}
	fmt.Println()

	// modulation uses much higher frequency, leading to smaller antennas
	fmt.Println("modulation frequency")
	modulate := []float64{30 * 1e3, 30 * 1e9, 40 * 1e9, 50 * 1e9, 64 * 1e9}
	for _, v := range modulate {
		fmt.Printf("%f hz: %f m\n", v, wavelength(v))
	}
}

func wavelength(hz float64) (m float64) {
	const c = 299792458
	return c / hz
}
