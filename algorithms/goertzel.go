// https://en.wikipedia.org/wiki/Goertzel_algorithm
// https://web.archive.org/web/20180628024641/http://en.dsplib.org/content/goertzel/goertzel.html
// provides an efficient way to calculate a DFT term if we don't care about the full spectrum
// it is slower than the FFT for the full frequency spectrum, but it is faster if we only care about a few terms
package main

import (
	"fmt"
	"math"
	"math/cmplx"
)

func main() {
	testrdft()
	testdtmf()
}

func testrdft() {
	x1 := fgen(1, 2, 3, 4, 5, 6, 17, 50, 32, 4, 4, 4, 4, 60, 65, 440)
	for i := 0.0; i <= 440.0; i++ {
		v, p := gdft1r(x1, i)
		if cmplx.Abs(v) >= 1e-8 {
			fmt.Println(i, cmplx.Abs(v), p)
		}
	}

	x2 := []float64{3, 2, 1, -1, 1, -2, -3, -2}
	v, p := gdft1r(x2, 1)
	fmt.Println(v, p, cmplx.Abs(v)*cmplx.Abs(v))
}

func testdtmf() {
	tab := []rune{
		'1', '2', '3', 'A',
		'4', '5', '6', 'B',
		'7', '8', '9', 'C',
		'*', '0', '#', 'D',
	}
	tone := []float64{697, 770, 852, 941, 1209, 1336, 1477, 1633}

	// each key on a touch tone pad generates 2 frequencies added together
	// this is an application of the goertzel algorithm as it can discriminate
	// which button we pressed by calculating a few DFT sample points (8 DFT sample in this case)

	// if we have a 8000 hz frequency spectrum and we sample 205 point for DFT analysis
	// (sampling at 1/8000 time steps every point), it means that 8000/25 gives us
	// the size of how much frequency is in each index bin (~39 frequency in one bin)
	// but since we don't need to discriminate frequencies near each other
	// we can get away with it and still detect if the tone is on or not
	F := 8000.0
	N := 205
	S := F / float64(N)
	for _, ch := range tab {
		f := dtmf(ch)
		x := sgen(F, N, f[0], f[1])

		var u []int
		for _, t := range tone {
			i := round(t / S)
			v, _ := gdft1r(x, i)
			if cmplx.Abs(v) >= float64(N/4) {
				u = append(u, int(i))
			}
		}
		if len(u) != 2 {
			panic("failed to detect tone")
		}
		fmt.Printf("%v=%v %v=%v\n", u[0], f[0], u[1], f[1])
	}
}

func round(x float64) float64 {
	return float64(int(x + 0.5))
}

func dtmf(ch rune) [2]float64 {
	tab := [][]rune{
		{'1', '2', '3', 'A'},
		{'4', '5', '6', 'B'},
		{'7', '8', '9', 'C'},
		{'*', '0', '#', 'D'},
	}
	tone1 := []float64{697, 770, 852, 941}
	tone2 := []float64{1209, 1336, 1477, 1633}

	for i := range tab {
		for j := range tab[i] {
			if ch == tab[i][j] {
				return [2]float64{tone1[i], tone2[j]}
			}
		}
	}
	return [2]float64{}
}

func sgen(rate float64, size int, hz ...float64) []float64 {
	x := make([]float64, size)
	t := 0.0
	dt := 1 / rate
	for i := 0; i < size; i++ {
		for j := range hz {
			w := 2 * math.Pi * hz[j]
			x[i] += math.Sin(w * t)
		}
		t += dt
	}
	return x
}

func fgen(hz ...float64) []float64 {
	maxhz := 0.0
	for i := range hz {
		maxhz = math.Max(maxhz, hz[i])
	}

	x := make([]float64, int(2*maxhz+1))
	for i := range x {
		for j := range hz {
			w := 2 * math.Pi * hz[j]
			t := float64(i) / float64(len(x))
			x[i] += math.Cos(w * t)
		}
	}
	return x
}

// calculate 1 dft term for real input data
func gdft1r(x []float64, k float64) (val complex128, power float64) {
	var s1, s2 float64

	w := 2 * math.Pi * k / float64(len(x))
	ci, cr := math.Sincos(w)
	for i := range x {
		y := x[i] + 2*cr*s1 - s2
		s1, s2 = y, s1
	}

	val = complex(s1*cr-s2, s1*ci)
	power = s2*s2 + s1*s1 - 2*cr*s1*s2
	return
}
