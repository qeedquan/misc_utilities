/*

https://www.kalmanfilter.net/alphabeta.html

Given a radar tracking an aircraft moving in the x direction
with a constant acceleration at a starting position, figure out
the next position, velocity, and acceleration

*/

package main

import (
	"fmt"
	"math"
	"math/rand"
	"time"
)

func main() {
	rand.Seed(time.Now().UnixNano())

	// this one is not actually constant acceleration
	// starts with constant velocity for 15 seconds (zero acceleration)
	// then switch to constant acceleration for 35 seconds
	m0 := mklutf([]float64{
		30160, 30365, 30890, 31050, 31785,
		32215, 33130, 34510, 36010, 37265,
	})

	x0 := 30250.0
	v0 := 50.0
	a0 := 0.0
	r0 := mkrandf(x0, v0, a0, 100)

	// the rule of thumb is that high gain implies you trust the sensors more, and low gain means less trust
	// since we assume the model is constant acceleration we assume low gain for acceleration but other gains will be higher
	test(50, x0, v0, a0, 0.5, 0.4, 0.1, m0)
	test(100, x0, v0, a0, 0.6, 0.7, 0.05, r0)

	// high gain for acceleration leads to big errors
	test(100, x0, v0, a0, 0.6, 0.7, 0.5, r0)
}

func test(n int, x0, v0, a0, g, h, k float64, measure func(float64) float64) {
	p := state{
		g:  g,
		h:  h,
		k:  k,
		x:  x0,
		v:  v0,
		a:  a0,
		xn: x0,
		vn: v0,
		an: a0,
	}
	dt := 5.0

	fmt.Printf("iters %d alpha %.2f beta %.2f gamma %.2f\n", n, g, h, k)
	for i := 0; i < n; i++ {
		t := float64(i) * dt
		r := x0 + v0*t + a0*t*t*0.5
		z := measure(t)
		p.update(z, dt)
		fmt.Printf("%d r %.1f z %.1f x %.1f v %.1f a %.1f xn %.1f vn %.1f an %.1f err %.6f\n",
			i+1, r, z, p.x, p.v, p.a, p.xn, p.vn, p.an, math.Abs(r-p.x))
	}
	fmt.Println()
}

func mkrandf(r, v, a, d float64) func(float64) float64 {
	return func(t float64) float64 {
		s := 2*rand.Float64() - 1
		return r + v*t + 0.5*t*t*a + s*d
	}
}

func mklutf(tab []float64) func(float64) float64 {
	i := 0
	return func(float64) float64 {
		r := tab[i]
		i = (i + 1) % len(tab)
		return r
	}
}

type state struct {
	x, v, a    float64
	xn, vn, an float64
	g, h, k    float64
}

// alpha-beta-gamma filters (also known as ghk filters) are just an extension on the alpha-beta filter
// except the gamma value is used for the third variable we want to extrapolate
// which is acceleration in this case, we update the variable based on our gain
// and then feed it back into the model equation of motion to extrapolate where we
// are going to go next
func (p *state) update(z, dt float64) {
	// use our extrapolated estimate as our current position
	p.x = p.xn
	p.v = p.vn
	p.a = p.an

	// update the model with measurement using our gain to adjust it
	p.a += p.k * ((z - p.x) / (0.5 * dt * dt))
	p.v += p.h * ((z - p.x) / dt)
	p.x += p.g * (z - p.x)

	// feed it into our equation of motion to figure out where we are going next
	p.xn = p.x + dt*p.v + 0.5*dt*dt*p.a
	p.vn = p.v + dt*p.a
	p.an = p.a
}
