/*

https://liquidsdr.org/blog/pll-howto/
https://liquidsdr.org/blog/pll-simple-howto/

*/

package main

import (
	"fmt"
	"math"
	"math/cmplx"
)

func main() {
	fmt.Println(findpll(5.3, -0.20, 0.05, 1000))
}

/*

Problem statement
I have an incoming complex sinusoid with an unknown but constant frequency and phase (and it's possibly noisy, but we won't add noise in our example).
I want to track the phase of this input on a sample-by-sample basis. Basically I need to generate a new sinusoid (the output) that has the exact same phase and frequency as the input.

The concept of a PLL is simple:

Make a measurement of the phase error between the input signal and my output sinusoid.
Adjust my output signal's frequency and phase proportional to this phase error.
Repeat.

Define a variable α as the the proportion of the phase error we will apply to adjust our output phase.
Note that α is proportional to the loop filter bandwidth and so it should be relatively small (e.g. 0.05 or so).
Define a new variable β=α^2/2
This value will be the proportion of the phase error we will apply to adjust our output frequency.

*/

func findpll(phase_in, frequency_in, alpha0 float64, samples int) (alpha, beta, frequency_out, phase_out, phase_error float64) {
	alpha = alpha0
	beta = 0.5 * alpha * alpha
	phase_error = math.MaxFloat64

	for range samples {
		signal_in := cmplx.Exp(complex(0, phase_in))
		signal_out := cmplx.Exp(complex(0, phase_out))
		phase_error = cmplx.Phase(signal_in * cmplx.Conj(signal_out))

		phase_out += alpha * phase_error
		frequency_out += beta * phase_error
		phase_in += frequency_in
		phase_out += frequency_out
	}
	return
}
