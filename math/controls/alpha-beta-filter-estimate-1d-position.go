/*

https://www.kalmanfilter.net/alphabeta.html

Given a radar tracking an aircraft moving in the x direction
with a constant velocity at a starting position, figure out
the next position and velocity

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

	m0 := mklutf([]float64{
		30110, 30265, 30740, 30750, 31135,
		31015, 31180, 31610, 31960, 31865,
	})
	r0 := 10290.0
	v0 := 40.0

	test(10, 30200, 40, 0.2, 0.1, m0)
	test(10, 30200, 40, 0.8, 0.5, m0)

	test(50, r0, v0, 0.2, 0.1, mkrandf(r0, v0, 10))
	test(100, r0, v0, 0.5, 0.5, mkrandf(r0, v0, 500))
	test(100, r0, v0, 0.3, 1.0, mkrandf(r0, v0, 50))
	test(1000, r0, v0, 0.8, 1.0, mkrandf(r0, v0, 50))
}

func test(n int, x0, v0, a, b float64, measure func(float64) float64) {
	p := state{
		a:  a,
		b:  b,
		x:  x0,
		v:  v0,
		xn: x0,
		vn: v0,
	}
	dt := 5.0

	fmt.Printf("iters %d alpha %.2f beta %.2f\n", n, a, b)
	for i := 0; i < n; i++ {
		t := float64(i) * dt
		r := x0 + v0*t
		z := measure(t)
		p.update(z, dt)
		fmt.Printf("%d r %.1f z %.1f x %.1f v %.1f xn %.1f vn %.1f err %.6f\n",
			i+1, r, z, p.x, p.v, p.xn, p.vn, math.Abs(r-p.x))
	}
	fmt.Println()
}

func mkrandf(r, v, d float64) func(float64) float64 {
	return func(t float64) float64 {
		s := 2*rand.Float64() - 1
		return r + v*t + s*d
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
	x, v   float64
	xn, vn float64
	a, b   float64
}

// alpha-beta filters are just an extension on the alpha filter
// except the beta value is used for the second variable we want to extrapolate
// which is velocity in this case, we update the variable based on our gain
// and then feed it back into the model equation of motion to extrapolate where we
// are going to go next
func (p *state) update(z, dt float64) {
	// use our extrapolated estimate as our current position
	p.x = p.xn
	p.v = p.vn

	// update the model with measurement using our gain to adjust it
	p.v += p.b * ((z - p.x) / dt)
	p.x += p.a * (z - p.x)

	// feed it into our equation of motion to figure out where we are going next
	p.xn = p.x + dt*p.v
	p.vn = p.v
}
