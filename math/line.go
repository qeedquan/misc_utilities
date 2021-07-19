package main

import (
	"fmt"
	"math/rand"

	"github.com/qeedquan/go-media/math/f64"
)

func main() {
	for i := 0; i < 1000; i++ {
		lineThroughPoint(f64.Vec2{500, 340}, nil, nil, 1)
		lineThroughPoint(f64.Vec2{500, 340}, nil, nil, 2)
	}

	lineFrom2Points(f64.Vec2{340, 250}, f64.Vec2{-14, 5})

	variousSlopesFromViewport(1024, 768)
	variousSlopesFromViewport(1280, 1024)
	variousSlopesFromViewport(1920, 1080)
}

// Given a point, we want to find some lines through that point
// the equation for a line is y=mx + b.
// The simplest way is to assume b=0 and then m=y/x for a line
// and many textbooks simply use that, but to find all lines
// through a point using the form y=mx + b is we have to let
// the 2 variables be free, but we can solve for any specific
// instance by randomizing 1 variable to fix it and solve for
// the other one
func lineThroughPoint(P f64.Vec2, M, B *float64, rnd uint) {
	switch {
	case M == nil && B == nil:
		M = new(float64)
		B = new(float64)
		// generate a random M or B and then solving for it
		switch rnd {
		case 1:
			*B = randf()
			*M = (P.Y - *B) / P.X
		case 2:
			*M = randf()
			*B = P.Y - *M*P.X
		}

	case M == nil:
		// y = mx + b
		// (y - b) / x = m
		M = new(float64)
		*M = (P.Y - *B) / P.X

	case B == nil:
		// y = mx + b
		// y - mx = b
		B = new(float64)
		*B = P.Y - *M*P.X
	}

	fmt.Printf("%f = (%f)*%f + %f\n", P.Y, *M, P.X, *B)
}

func randf() float64 {
	return rand.Float64()*1e4 - 5e3
}

// Since we solve for m in this case, b is already fixed
// once we solved for m, we just don't know it yet
// as shown below B1 and B2 are the same
// (y2 - y1) / (x2 - x1) = m
// y2 = m(x2) + b
// y1 = m(x1) + b
// b1 = y1 - m(x1)
// b2 = y2 - m(x2)
// b1 = b2 since m is already fixed, so b is constrained
// by m to be only 1 value
// we can also write a line equation as
// y = y1 + (y2-y1)/(x2-x1)*(x-x1), in this case
// the equation will expand out to y=mx + b when you do
// the algebra since (x-x0) will be 0 when x=x0 so plugging in
// x0 gives you y0
func lineFrom2Points(P, Q f64.Vec2) {
	M := (Q.Y - P.Y) / (Q.X - P.X)
	B1 := Q.Y - M*Q.X
	B2 := P.Y - M*P.X
	fmt.Printf("\n")
	fmt.Printf("%v = (%v)*%v + %v\n", P.Y, M, P.X, B1)
	fmt.Printf("%v = (%v)*%v + %v\n", Q.Y, M, Q.X, B2)
	fmt.Printf("y = %vx + %v\n", M, B1)
	fmt.Printf("y = %v(x-%v) + %v\n", M, P.X, P.Y)
	fmt.Printf("\n")
}

func variousSlopesFromViewport(w, h int) {
	// Given a viewport, largest slope we can get is
	// by optimizing (y2-y1)/(x2-x1), since
	// we have integer values from coordinates, the smallest
	// value x2-x1 can take is 1, and the largest y2-y1 value
	// is the topmost to the botto most, or the better known
	// as the height
	// h is (y2-y1)
	// 1 is (x2-x1)
	largestSlope := float64(h) / 1

	// If we want a slope that spans a 45 degree angle
	// as it travels through the rectangle, the slope will be
	// h/w because y1=0, x1=0, x2=w, y2=h, using the
	// formula (y2 - y1) / (x2 - x1) = m and simplifying we get
	// h/w
	middleSlope := float64(h) / float64(w)

	// due to the fact that h/w on regular display are usually less than 1
	// (3/4) or 9/16 and so on, the slope is less than 1 if we move left to
	// right

	fmt.Printf("largest slope = %f\n", largestSlope)
	fmt.Printf("middle slope = %f\n", middleSlope)
}

// When drawing lines, either abs(dy/dx) <= 1 or abs(dx/dy) <= 1
// we want to pick the one axis where the slope is less than or equal to 1
// and iterate one by one pixel through that while incrementing the slope
// that is how bresenham line algorithm works.
// http://kt8216.unixcab.org/murphy/index.html
