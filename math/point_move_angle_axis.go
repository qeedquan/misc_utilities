// measure the angle between the point and the 3 basis
// we treat it as vector when we measure the angle though
// but position in space matters here
package main

import (
	"fmt"
	"math/rand"
	"time"

	"github.com/qeedquan/go-media/math/ga"
	"github.com/qeedquan/go-media/math/ga/vec3"
)

func main() {
	rand.Seed(time.Now().UnixNano())

	X := ga.Vec3d{1, 0, 0}
	Y := ga.Vec3d{0, 1, 0}
	Z := ga.Vec3d{0, 0, 1}

	// when we move along the axis aligned
	// the angles are always 90 degrees or 0
	// in other words the angle between the vector
	// are unchanged when you move in one or the other
	// direction on that an aligned axis
	fmt.Println("aligned movement")
	p := ga.Vec3d{}
	for i := float64(0); i < 32; i++ {
		p.X += i
		measure(X, Y, Z, p)
	}
	fmt.Println()

	p = ga.Vec3d{}
	for i := float64(0); i < 32; i++ {
		p.Y += i
		measure(X, Y, Z, p)
	}

	p = ga.Vec3d{}
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
	p = ga.Vec3d{0, 0, 1000}
	for i := float64(0); i < 32; i++ {
		p.X += i
		measure(X, Y, Z, p)
	}
	fmt.Println()

	p = ga.Vec3d{1, 0, 0}
	for i := float64(0); i < 32; i++ {
		p.Y += i
		measure(X, Y, Z, p)
	}

	p = ga.Vec3d{0, 1, 0}
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

func measure(X, Y, Z, p ga.Vec3d) {
	ax := angle(p, X)
	ay := angle(p, Y)
	az := angle(p, Z)
	fmt.Println(X, Y, Z, ax, ay, az, p)
}

func angle(p, q ga.Vec3d) float64 {
	return ga.Rad2Deg(vec3.Angle(p, q))
}

func randVec(n float64) ga.Vec3d {
	return ga.Vec3d{
		rand.Float64() * n,
		rand.Float64() * n,
		rand.Float64() * n,
	}
}
