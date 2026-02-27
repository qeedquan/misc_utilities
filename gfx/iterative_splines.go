// https://keithp.com/blogs/iterative-splines/
// https://keithp.com/blogs/more-iterative-splines/
package main

import (
	"fmt"
)

type (
	Point  [2]float64
	Spline [4]Point
)

func main() {
	spline := Spline{
		{0.0, 0.0},
		{0.0, 256.0},
		{256.0, -256.0},
		{256.0, 0.0},
	}
	drawspline1(Printer{}, spline)
	fmt.Println()
	drawspline2(Printer{}, spline)
}

const (
	DRAW_TOLERANCE = 0.5
	FLAT_TOLERANCE = 0.01
)

// This actually returns flatness² * 16,
// so we need to compare against scaled values
// using the SCALE_FLAT macro
func flatness(spline Spline) float64 {
	// This computes the maximum deviation of the spline from a
	// straight line between the end points.
	// From https://hcklbrrfnn.files.wordpress.com/2012/08/bez.pdf

	ux := 3.0*spline[1][0] - 2.0*spline[0][0] - spline[3][0]
	uy := 3.0*spline[1][1] - 2.0*spline[0][1] - spline[3][1]
	vx := 3.0*spline[2][0] - 2.0*spline[3][0] - spline[0][0]
	vy := 3.0*spline[2][1] - 2.0*spline[3][1] - spline[0][1]

	ux *= ux
	uy *= uy
	vx *= vx
	vy *= vy
	if ux < vx {
		ux = vx
	}
	if uy < vy {
		uy = vy
	}

	// If we wanted to return the true flatness, we'd use:
	// return sqrtf((ux + uy)/16.0f)
	return ux + uy
}

// Convert constants to values usable with flatness()
func SCALE_FLAT(f float64) float64 {
	return f * f * 16
}

func isflat(spline Spline) bool {
	return flatness(spline) <= 16.0*DRAW_TOLERANCE*DRAW_TOLERANCE
}

func lerp(a, b Point, t float64) Point {
	var r Point
	for i := 0; i < 2; i++ {
		r[i] = a[i]*(1.0-t) + b[i]*t
	}
	return r
}

func decasteljau(s, s1, s2 Spline, t float64) (Spline, Spline) {
	var (
		first  [3]Point
		second [2]Point
	)

	for i := 0; i < 3; i++ {
		first[i] = lerp(s[i], s[i+1], t)
	}
	for i := 0; i < 2; i++ {
		second[i] = lerp(first[i], first[i+1], t)
	}

	s1[3] = lerp(second[0], second[1], t)

	for i := 0; i < 2; i++ {
		s1[0][i] = s[0][i]
		s1[1][i] = first[0][i]
		s1[2][i] = second[0][i]

		s2[0][i] = s1[3][i]
		s2[1][i] = second[1][i]
		s2[2][i] = first[2][i]
		s2[3][i] = s[3][i]
	}
	return s1, s2
}

func drawspline1(m Drawer, s Spline) {
	var s1, s2 Spline

	m.Set(s[0][0], s[0][1])
	for !isflat(s) {
		t := 1.0
		for {
			t = t / 2.0
			s1, s2 = decasteljau(s, s1, s2, t)
			if isflat(s1) {
				break
			}
		}
		m.Set(s1[3][0], s1[3][1])
		s = s2
	}
	m.Set(s[3][0], s[3][1])
}

// Decompose 's' into straight lines which are
// within DRAW_TOLERANCE of the spline
func drawspline2(m Drawer, s Spline) {
	// Start at the beginning of the spline.
	m.Set(s[0][0], s[0][1])

	// Split the spline until it is flat enough
	for flatness(s) > SCALE_FLAT(DRAW_TOLERANCE) {
		var s1, s2 Spline
		hi := 1.0
		lo := 0.0

		// Search for an initial section of the spline which
		// is flat, but not too flat
		for {
			// Average the lo and hi values for our
			// next estimate
			t := (hi + lo) / 2.0

			// Split the spline at the target location
			s1, s2 = decasteljau(s, s1, s2, t)

			// Compute the flatness and see if s1 is flat enough
			flat := flatness(s1)

			if flat <= SCALE_FLAT(DRAW_TOLERANCE) {
				// Stop looking when s1 is close
				// enough to the target tolerance
				if flat >= SCALE_FLAT(DRAW_TOLERANCE-FLAT_TOLERANCE) {
					break
				}

				lo = t
			} else {
				hi = t
			}
		}

		m.Set(s1[3][0], s1[3][1])
		s = s2
	}
	m.Set(s[3][0], s[3][1])
}

type Drawer interface {
	Set(x, y float64)
}

type Printer struct{}

func (p Printer) Set(x, y float64) {
	fmt.Printf("%8f %8f\n", x, y)
}
