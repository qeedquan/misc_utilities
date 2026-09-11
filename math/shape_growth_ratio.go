// ratios of shapes as they increase in size
package main

import (
	"fmt"
	"image"
	"math"
)

func main() {
	square()
	circle()
}

func square() {
	r := image.Rect(0, 0, 1, 1)
	pp := 0
	pa := 0
	for i := 0; i < 32; i++ {
		nr := r.Inset(-i)
		a := nr.Dx() * nr.Dy()
		p := nr.Dx() * 4
		fmt.Printf("%d side %d perimeter %d area %d diff_perim %d diff_area %d ratio_perim %f ratio_area %f\n",
			i, p/4, p, a, p-pp, a-pa, float64(p)/float64(pp), float64(a)/float64(pa))
		pp = p
		pa = a
	}
	fmt.Println()
}

func circle() {
	pa := 0.0
	pc := 0.0
	for i := 0; i < 32; i++ {
		r := float64(i)
		c := 2 * math.Pi * r
		a := math.Pi * r * r
		fmt.Printf("%d circumference %f diff_circumference %f ratio_circumference %f area %f diff_area %f ratio_area %f\n",
			i, c, c-pc, c/pc, a, a-pa, a/pa)
		pa = a
		pc = c
	}
	fmt.Println()
}
