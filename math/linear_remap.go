package main

import (
	"fmt"
	"math/rand"
	"time"

	"github.com/qeedquan/go-media/math/f64"
)

func main() {
	rand.Seed(time.Now().UnixNano())
	ex1()
	ex2()
}

// OpenGL and other rendering APIs expect
// coordinate system to be between [-1, 1]
// and we need to remap it to [0,W]x[0,H] at
// the end this is the standard equation of remapping

// [-1, 1] -> [0, W]
// [-1, 1] -> [0, H]
// [0, 1] -> [0, W]
// [0, 1] -> [0, H]
func ex1() {
	fmt.Printf("EX1:\n\n")
	w := 1024.0
	h := 768.0
	for i := 0; i < 10; i++ {
		// make a random value between [-1, 1]
		x := rand.Float64()*2 - 1
		y := rand.Float64()*2 - 1

		hw := w / 2
		hh := h / 2

		px := x*hw + hw
		py := y*hh + hh

		xx := f64.LinearRemap(x, -1, 1, 0, 1)
		yy := f64.LinearRemap(y, -1, 1, 0, 1)
		ppx := xx * w
		ppy := yy * h

		fmt.Printf("(%f, %f) (%f, %f)\n", x, y, px, py)
		fmt.Printf("(%f, %f) (%f, %f)\n", xx, yy, ppx, ppy)
		fmt.Printf("\n")
	}
}

// If we add a perspective divide into the mix, the
// linear remapping from [-1, 1] -> [0, 1] and then
// calculating the coordinate based on that are not
// equivalent as the projection back into screen space shows.
// This is due to a sign flip from [-1, 1], if the mapping
// was from [0, 1] -> [0, C] where the range did not flip
// sign, then the value produce are the same, but they are
// different ratio values when dealing with a sign flip

// If you were to draw this out with a set of points and
// vary the z parameter, the [-1, 1] center is correct
// as it focuses on 0,0 as center of screen, if you do
// [0, 1] then center is top left when drawn top-left to bottom-right

// [-C, C] -> [-1, 1] will work as example 1 because there is no sign flip

// More generally speaking, if the left bound and right bound are scaled
// by a constant when linear remapping to a new range, then the element division
// are the same, if the left side scaling ratio is different from the right side
// then the division will produce different results, ie [-1000, 50] -> [-1, 1]
// will have different result even though they are the same signs
func ex2() {
	fmt.Printf("EX2:\n\n")
	w := 1280.0
	h := 800.0
	for i := 0; i < 10; i++ {
		x := rand.Float64()*2 - 1
		y := rand.Float64()*2 - 1
		z := rand.Float64()*2 - 1

		hw := w / 2
		hh := h / 2

		px := (x/z)*hw + hw
		py := (y/z)*hh + hh

		xx := f64.LinearRemap(x, -1, 1, 0, 1)
		yy := f64.LinearRemap(y, -1, 1, 0, 1)
		zz := f64.LinearRemap(z, -1, 1, 0, 1)

		ppx := (xx / zz) * w
		ppy := (yy / zz) * h

		x1 := x / z
		y1 := y / z

		x2 := xx / zz
		y2 := yy / zz

		fmt.Printf("(%f, %f) (%f, %f)\n", x, y, px, py)
		fmt.Printf("(%f, %f) (%f, %f)\n", xx, yy, ppx, ppy)
		fmt.Printf("(%f, %f) (%f, %f)\n", x1, y1, x2, y2)
		fmt.Printf("\n")
	}
}
