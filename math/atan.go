package main

import (
	"fmt"
	"math"
)

func main() {
	testangpos()
	testangdist()
}

func testangpos() {
	t := 0.0
	for t <= 100*2*math.Pi {
		y, x := math.Sincos(t)
		p := math.Atan2(y, x)

		fmt.Printf("%.6f %.6f %.6f\n", rad2deg(t), rad2deg(p), rad2deg(angpos(p)))
		t += deg2rad(1)
	}
}

func testangdist() {
	tab := [][2]float64{
		{0, 719},
		{0, 180},
		{10, 350},
		{180, 360},
		{90, 0},
		{24, 510},
		{-70, 30},
		{-180, 180},
		{-148, -150},
	}

	for _, p := range tab {
		x := deg2rad(p[0])
		y := deg2rad(p[1])

		a := rad2deg(angdist(x, y))
		fmt.Printf("%.6f %.6f\n", p, a)
	}
}

func deg2rad(x float64) float64 {
	return x * math.Pi / 180
}

func rad2deg(x float64) float64 {
	return x * 180 / math.Pi
}

// maps [-pi, pi] to [0, 2pi]
func angpos(x float64) float64 {
	return math.Mod(x+2*math.Pi, 2*math.Pi)
}

// shortest distance
func angdist(a, b float64) float64 {
	d := math.Mod(b-a+math.Pi, 2*math.Pi) - math.Pi
	if d < -math.Pi {
		return math.Abs(2*math.Pi + d)
	}
	return math.Abs(d)
}
