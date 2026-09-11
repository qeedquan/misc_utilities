/*

https://en.wikipedia.org/wiki/World_Geodetic_System#WGS84
https://en.wikipedia.org/wiki/Earth_radius

*/

package main

import (
	"math"
)

func main() {
	testconv()
	testlola()
}

func testlola() {
	x, y := lola2meter(deg2rad(1), deg2rad(1))

	// measure in kilometers
	// 1 degree of lat/long movement is this number of km
	x /= 1000
	y /= 1000
	assert(math.Abs(x-111.3) < 1e-1)
	assert(math.Abs(y-110.6) < 1e-1)

	// if we multiply by 360 degrees, it gives the earths
	// horizontal and vertical circumference in km, this is close enough
	// to less than 300 km of the earth's documented circumference
	x *= 360
	y *= 360
	assert(math.Abs(x-40075) < 210)
	assert(math.Abs(y-40008) < 210)
}

func testconv() {
	const eps = 1e-6
	v := [][3]float64{
		{1, 0, 0},
		{0, 1, 0},
		{0, 0, 1},
		{1, 0, 1},
		{0.5, 0.3, 0.1},
		{-1, 2, 0.3},
	}
	for _, p := range v {
		x, y, z := sphere2cart(cart2sphere(p[0], p[1], p[2]))
		cmp(x, y, z, p[0], p[1], p[2], eps)

		x, y, z = cylinder2cart(cart2cylinder(p[0], p[1], p[2]))
		cmp(x, y, z, p[0], p[1], p[2], eps)
	}
}

func cmp(x0, y0, z0, x1, y1, z1, eps float64) {
	assert(math.Abs(x1-x0) < eps)
	assert(math.Abs(y1-y0) < eps)
	assert(math.Abs(z1-z0) < eps)
}

func assert(x bool) {
	if !x {
		panic("assertion failed")
	}
}

func cart2sphere(x, y, z float64) (r, t, p float64) {
	r = math.Sqrt(x*x + y*y + z*z)
	t = math.Pi / 2
	p = math.Pi / 2
	if x != 0 {
		t = math.Atan2(y, x)
	}
	if z != 0 {
		p = math.Atan2(math.Hypot(x, y), z)
	}
	return
}

func sphere2cart(r, t, p float64) (x, y, z float64) {
	sp, cp := math.Sincos(p)
	st, ct := math.Sincos(t)
	x = r * sp * ct
	y = r * sp * st
	z = r * cp
	return
}

func cart2cylinder(x, y, z float64) (p, t, zc float64) {
	p = math.Hypot(x, y)
	zc = z
	switch {
	case x == 0 && y == 0:
		t = 0
	case x >= 0:
		t = math.Asin(y / p)
	case x < 0:
		t = -math.Asin(y/p) + math.Pi
	}
	return
}

func cylinder2cart(p, t, zc float64) (x, y, z float64) {
	x = p * math.Cos(t)
	y = p * math.Sin(t)
	z = zc
	return
}

// GRS80 or WGS84 spheroid model at sea level at the equator
// correct to within 1 cm, lo/la is in radians
// measure how much meters one has to travel on the earth to move 1 degree in lat/long
func lola2meter(lo, la float64) (x, y float64) {
	x = 111412.84*math.Cos(lo) - 93.5*math.Cos(3*lo) + 0.118*math.Cos(5*lo)
	y = 111132.92 - 559.82*math.Cos(2*la) + 1.175*math.Cos(4*la) - 0.0023*math.Cos(6*la)
	return
}

func deg2rad(d float64) float64 { return d * math.Pi / 180 }
