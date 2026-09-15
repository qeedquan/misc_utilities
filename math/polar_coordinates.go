/*

https://en.wikipedia.org/wiki/Polar_coordinate_system
http://www.physics.rutgers.edu/~chiarito/quiz_sol_honorsphys_271/vector_notes_2d.pdf
https://math.stackexchange.com/questions/1365622/adding-two-polar-vectors
https://greatscottgadgets.com/sdr/7/

*/

package main

import (
	"fmt"
	"math"
	"math/cmplx"
	"math/rand"
	"time"
)

func main() {
	rand.Seed(time.Now().UnixNano())
	testOps()
	testAverage()
}

// the polar coordinate metric for measuring lengths and angles is the euclidean metric
// hence we expect all common operations to have analogous formulations
func testOps() {
	const eps = 1e-7
	for i := 0; i <= 1000; i++ {
		// lengths
		x1, y1 := v2r(1e4)
		x2, y2 := v2r(1e4)
		r1, t1 := c2p(x1, y1)
		r2, t2 := c2p(x2, y2)
		d1 := cdist(x1, y1, x2, y2)
		d2 := pdist(r1, t1, r2, t2)
		assert(math.Abs(d1-d2) < eps)

		// linear translation
		x3, y3 := ctranslate(x1, y1, x2, y2)
		r3, t3 := ptranslate(r1, t1, r2, t2)
		x4, y4 := p2c(r3, t3)
		assert(math.Abs(x4-x3) < eps)
		assert(math.Abs(y4-y3) < eps)

		// linear scaling
		s := rand.Float64() * 1e2
		x3, y3 = cscale(x1, y1, s)
		r3, t3 = pscale(r1, t1, s)
		x4, y4 = p2c(r3, t3)
		assert(math.Abs(x4-x3) < eps)
		assert(math.Abs(y4-y3) < eps)

		// dot product
		d1 = cdot(x1, y1, x2, y2)
		d2 = pdot(r1, t1, r2, t2)
		assert(math.Abs(d1-d2) < eps)

		// cross product (it's actually a wedge product in 2D)
		d1 = ccross(x1, y1, x2, y2)
		d2 = pcross(r1, t1, r2, t2)
		assert(math.Abs(d1-d2) < eps)
	}
}

// if we have a set of vectors with magnitude and direction that we would like to find the average of
// we can treat them as polar coordinates and apply the standard average formula in polar space,
// we can use the complex exponentials to make this easier, given us the formula:
// V = sum(r_j*exp(i*theta_j)) / N
// angle = imag(log(V))
// magnitude = abs(V)
func testAverage() {
	tab := [][2]float64{
		{1, deg2rad(12)},
		{1, deg2rad(15)},
		{1, deg2rad(13)},
		{1, deg2rad(9)},
		{1, deg2rad(16)},
	}

	x := pavg(tab)
	fmt.Println("magnitude", cmplx.Abs(x))
	fmt.Println("angle", rad2deg(imag(cmplx.Log(x))))
}

func assert(x bool) {
	if !x {
		panic("assertion failed")
	}
}

func v2r(s float64) (x, y float64) {
	x = rand.Float64() * s
	y = rand.Float64() * s
	return
}

func c2p(x, y float64) (r, t float64) {
	r = math.Hypot(x, y)
	t = math.Atan2(y, x)
	return
}

func p2c(r, t float64) (x, y float64) {
	x = r * math.Cos(t)
	y = r * math.Sin(t)
	return
}

func ctranslate(x1, y1, x2, y2 float64) (x, y float64) {
	x = x1 + x2
	y = y1 + y2
	return
}

func cscale(x, y, s float64) (xn, yn float64) {
	xn = x * s
	yn = y * s
	return
}

func cdot(x1, y1, x2, y2 float64) float64 {
	return x1*x2 + y1*y2
}

func ccross(x1, y1, x2, y2 float64) float64 {
	return x1*y2 - x2*y1
}

// distance between 2 points in euclidean is the pythagorean theorem
func cdist(x1, y1, x2, y2 float64) float64 {
	dx := x2 - x1
	dy := y2 - y1
	return math.Hypot(dx, dy)
}

// linear translation is much more complicated in polar coordinates
func ptranslate(r1, t1, r2, t2 float64) (r, t float64) {
	s, c := math.Sincos(t2 - t1)
	r = math.Sqrt(r1*r1 + r2*r2 + 2*r1*r2*c)
	t = t1 + math.Atan2(r2*s, r1+r2*c)
	return
}

// linear scaling preserve angles
func pscale(r, t, s float64) (rn, tn float64) {
	rn = r * s
	tn = t
	return
}

// distance between 2 points in polar form is the law of cosines
func pdist(r1, t1, r2, t2 float64) float64 {
	return math.Sqrt(r1*r1 + r2*r2 - 2*r1*r2*math.Cos(t2-t1))
}

func pdot(r1, t1, r2, t2 float64) float64 {
	return r1 * r2 * math.Cos(t2-t1)
}

func pcross(r1, t1, r2, t2 float64) float64 {
	return r1 * r2 * math.Sin(t2-t1)
}

// average a set of vectors that are treated as polar coordinates
func pavg(a [][2]float64) complex128 {
	x := 0i
	for _, v := range a {
		r := complex(v[0], 0)
		t := complex(0, v[1])
		x += r * cmplx.Exp(t)
	}
	x /= complex(float64(len(a)), 0)
	return x
}

func deg2rad(x float64) float64 { return x * math.Pi / 180 }
func rad2deg(x float64) float64 { return x * 180 / math.Pi }
