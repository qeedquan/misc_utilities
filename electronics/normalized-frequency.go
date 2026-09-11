/*

https://en.wikipedia.org/wiki/Nyquist_frequency
https://www.physik.uzh.ch/local/teaching/SPI301/LV-2015-Help/lvanlsconcepts.chm/Normalized_Frequency.html

The notation that is commonly used is the following:
fs   - sample rate
fs/2 - nyquist frequency
f    - [0, fs/2] (frequencies we can sample from the system without aliasing)
t    - [0, 1] (time scaled by 2*pi, so 1 represents a full period of the wave)

If f above the nyquist frequency [fs/2, fs], then the output is a reflection of [0, fs/2].
For frequencies greater than fs, the output will wrap around frequency wise (aliasing)

Normalized frequency is the mapping of mapping [0, fs] to [0, 1]
f    - [0, 1]
fs/2 - 0.5 (nyquist frequency)
fs   - 1

For frequency response we want to sample the following range of frequencies:
f - [0, fs/2]

If we do it in the normalized frequency form then this forces f to be:
f - [0, 0.5]

So now we have scale t by fs so that we can get:
t - [0, fs/2]

*/

package main

import (
	"fmt"
	"math"
)

func main() {
	fs := 20.0
	dt := 1 / (fs / 2)
	df := 1 / fs

	x := []float64{}
	for f := 0.0; f <= fs/2; f += 1 {
		for t := 0.0; t <= 1; t += dt {
			x = append(x, math.Cos(2*math.Pi*t*f))
		}
	}

	y := []float64{}
	for f := 0.0; f <= 0.5; f += df {
		for t := 0.0; t <= 1.0; t += dt {
			y = append(y, math.Cos(2*math.Pi*t*fs*f))
		}
	}

	for i := range min(len(x), len(y)) {
		fmt.Println(x[i], y[i])
	}
	fmt.Println(len(x), len(y))
}
