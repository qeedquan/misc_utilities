// measure the angle between the point and the 3 basis
// we treat it as vector when we measure the angle though
// but position in space matters here
package main

import (
	"fmt"
	"math/rand"
	"time"

	"github.com/qeedquan/go-media/math/f64"
)

func main() {
	rand.Seed(time.Now().UnixNano())

	X := f64.Vec3{1, 0, 0}
	Y := f64.Vec3{0, 1, 0}
	Z := f64.Vec3{0, 0, 1}

	// when we move along the axis aligned
	// the angles are always 90 degrees or 0
	// in other words the angle between the vector
	// are unchanged when you move in one or the other
	// direction on that an aligned axis
	fmt.Println("aligned movement")
	p := f64.Vec3{}
	for i := float64(0); i < 32; i++ {
		p.X += i
		measure(X, Y, Z, p)
	}
	fmt.Println()

	p = f64.Vec3{}
	for i := float64(0); i < 32; i++ {
		p.Y += i
		measure(X, Y, Z, p)
	}

	p = f64.Vec3{}
	fmt.Println()
	for i := float64(0); i < 32; i++ {
		p.Z += i
		measure(X, Y, Z, p)
	}

	// when we move along the axis aligned
	// only for one axis, only one 90
	// degree basis is preserved between
	// the point and that basis
	fmt.Println()
	fmt.Println("one aligned axis movement")
	p = f64.Vec3{0, 0, 1000}
	for i := float64(0); i < 32; i++ {
		p.X += i
		measure(X, Y, Z, p)
	}
	fmt.Println()

	p = f64.Vec3{1, 0, 0}
	for i := float64(0); i < 32; i++ {
		p.Y += i
		measure(X, Y, Z, p)
	}

	p = f64.Vec3{0, 1, 0}
	fmt.Println()
	for i := float64(0); i < 32; i++ {
		p.Z += i
		measure(X, Y, Z, p)
	}

	// if the point is not aligned on any axis,
	// moving on one direction affects all the angle
	// of all axis, this is how perspective projection (x/z, y/z)
	// looks different when you move farther away in z and keep x and y
	// fixed, the angle between the vectors changed
	p = randVec(256)
	fmt.Println()
	fmt.Println("non-aligned axis movement")
	for i := float64(0); i < 32; i++ {
		p.X += i
		measure(X, Y, Z, p)
	}

	p = randVec(256)
	fmt.Println()
	for i := float64(0); i < 32; i++ {
		p.Y += i
		measure(X, Y, Z, p)
	}

	p = randVec(256)
	fmt.Println()
	for i := float64(0); i < 32; i++ {
		p.Z += i
		measure(X, Y, Z, p)
	}
}

func measure(X, Y, Z, p f64.Vec3) {
	ax := angle(p, X)
	ay := angle(p, Y)
	az := angle(p, Z)
	fmt.Println(X, Y, Z, ax, ay, az, p)
}

func angle(p, q f64.Vec3) float64 {
	return f64.Rad2Deg(p.Angle(q))
}

func randVec(n float64) f64.Vec3 {
	return f64.Vec3{
		rand.Float64() * n,
		rand.Float64() * n,
		rand.Float64() * n,
	}
}
