package main

import (
	"fmt"
	"math"
)

func main() {
	fmt.Println(rad2hz(200))
	fmt.Println(hz2rad(rad2hz(452)))
	fmt.Println(rad2hz(800))
	fmt.Println(rad2deg(deg2rad(360)))
	fmt.Println(rad2hz(5000))
	fmt.Println(rad2hz(2 * math.Pi))
	fmt.Println(rad2hz(1))
	fmt.Println(rad2deg(1))
	fmt.Println(rad2rpm(1))
	fmt.Println(rad2rpm(2.1))
	fmt.Println(rad2rpm(3.1))
	fmt.Println(rad2rpm(7.3))
	fmt.Println(rad2rpm(10.5))
	fmt.Println(rpm2rad(20))
	fmt.Println(rpm2rad(100.5))
	fmt.Println(rpm2rad(rad2rpm(45.3520)))
}

// radian worth of rotation per minute
func rad2rpm(r float64) float64 {
	return (r * 60) / (2 * math.Pi)
}

func rpm2rad(r float64) float64 {
	return (r * 2 * math.Pi) / 60
}

// angular frequency to hz per second
func rad2hz(r float64) float64 {
	return r / (2 * math.Pi)
}

func hz2rad(h float64) float64 {
	return h * 2 * math.Pi
}

// degree to radians
func deg2rad(d float64) float64 {
	return d * math.Pi / 180
}

func rad2deg(r float64) float64 {
	return r * 180 / math.Pi
}
