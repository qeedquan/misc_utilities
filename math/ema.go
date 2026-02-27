/*

https://en.wikipedia.org/wiki/Moving_average#Exponential_moving_average
https://en.wikipedia.org/wiki/Exponential_smoothing
https://en.wikipedia.org/wiki/Leaky_integrator

*/

package main

import (
	"fmt"
	"math/rand"
)

func main() {
	var E EMA
	E.SetAlpha(0.99)
	for i := float64(0); i < 100; i++ {
		fmt.Println(i, E.Update(i))
	}

	E.Reset()
	E.SetAlpha(0.6)
	for i := 0; i < 1000; i++ {
		v := rand.Float64()
		fmt.Println(v, E.Update(v))
	}
}

type EMA struct {
	once  bool
	alpha float64
	value float64
}

func (c *EMA) Reset() {
	c.once = false
}

func (c *EMA) Alpha() float64 {
	return c.alpha
}

func (c *EMA) SetAlpha(a float64) {
	c.alpha = a
}

// 0 <= alpha <= 1
// more alpha is closer to 1, the more it give precedence to new values
// s[0] = x[0]
// s[t] = a*x[t] + (1-a)*s[t-1] -> can be simplified to
// s[t] = s[t-1] + a*(x[t] - s[t-1])
// this can be viewed as a 1st order low pass filter with a cutoff frequency
// that is very small

// another convention is to define alpha' as 1-alpha and use alpha'
// in the calculation. The closer alpha' is to 0,
// the more precedence it gives to new values

// for time constant, the relationship between
// the time constant tau and smoothing alpha is given by
// 1 - e^(-T/tau) where T is the sampling time, if T is much smaller
// than tau, then alpha ~ T/tau
// this can be viewed as a leaky integrator where the we are computing the
// integral of the input but it "leaks" the input out gradually over time
// dx/dt = -Ax + C
// x(t) = exp(k*-A*t) + x0
func (c *EMA) Update(v float64) float64 {
	if !c.once {
		c.once = true
		c.value = v
	} else {
		c.value = c.value + c.alpha*(v-c.value)
	}

	return c.value
}
