// https://lpsa.swarthmore.edu/BackGround/phasor/phasor.html
// https://www.usna.edu/ECE/ee301/Lecture%20Notes/EE301%20Fall%202016%20Lesson%2015%20Phasors%20and%20Complex%20Numbers%20in%20AC%20and%20Impedance.pdf
package main

import (
	"fmt"
	"math"
	"math/rand"
	"time"

	"github.com/qeedquan/go-media/math/f64"
)

// a phasor is made up of amplitude and phase
// the wavelength is implicit since phasors
// only interact with other phasors of the same
// wavelength, but store wavelength to have
// complete wave information

// a phasor is based on euler identity
// A*exp(i*theta) = A*(cos(theta) + i*sin(theta))
// define phasor as F = A<theta
// to multiply the phasor by A*exp(i*theta)*exp(i*w*t) = F<(wt + theta)
// so a phasor can be viewed as a complex number represented in its exponential form

// from the above definitions,
// a phasor can also be represented as polar coordinates
// where we store the phase of the phasor in the theta term of the polar coordinate
// multiplication by exp(i*w*t) is a polar coordinate multiplication of polar(r=1, t=wt)
// and recover the time domain function by converting it back to rectangular coordinates
// and taking the x component

// one can draw a phasor diagram as a vector inside a vector of magnitude A pointing at location P (phase)
// varying the wavelength does not affect the phasor, what it does is if we plug in time and see how the
// phasor evolves according to A*cos(Wt + P), we can see the vector will move faster/slower based on
// W (this is expressed by the multiplication of exp(i*w*t) with the phasor)

// Since a phasor represents f(t) = A*cos(w*t + theta)
// differentiating f(t) is differentiating the rotating phasor vector F, which gives i*w*F
// integration of phasors gives 1/(i*w)*F

// phasors can be used to solve for varying current/voltage easier
// than applying calculus since it turns differentiation/integration
// into an algebra problem

// the general approach to representing v(t) or i(t) as a phasor is the following:
// 1) looking at waveform, determine the Vpeak and phase offset theta
// 2) Vrms = Vpeak/sqrt(2)
// 3) phasor is then Vrms<theta

// the impedance of an inductor can be written as (in polar or rectangular form)
// Zl = wL<90 = jwL

type Phasor struct {
	A, P, W float64
}

func main() {
	rand.Seed(time.Now().UnixNano())
	example1()
}

func example1() {
	// we want to write multiple cosine of different amplitudes and phases
	// as one cosine function

	// have 3 sinusoidal signals of same frequency
	// v(t) = 5*cos(200*t + 30 degrees) + 2*cos(200*t + 45 degrees) + cos(200*t - 60 degrees)
	// convert to phasor notation
	// get 5<30 + 2<45 + 1<-60
	// use exp(ix) = cos(x) + isin(x) and get
	// X: 5cos(30) + 2cos(45) + cos(-60) ~ 6.2443
	// Y: 5sin(30) + 2sin(45) + sin(-60) ~ 3.04819
	fmt.Println("Combining multiple cosine functions with same frequency")
	x := 5*cosd(30) + 2*cosd(45) + cosd(-60)
	y := 5*sind(30) + 2*sind(45) + sind(-60)
	fmt.Println(x, y)

	// this value can be written as a phasor
	// hence the one cosine function we want is
	// 6.948614275189787*cos(200*t + 26.0194154126505)
	p := Phasor{
		A: math.Hypot(x, y),
		P: f64.Rad2Deg(math.Atan2(y, x)),
		W: 200,
	}
	fmt.Println(p, x, p.A*cosd(p.P))

	for w := float64(0); w < 100; w += 0.01 {
		s := 0.0
		x := 0.0
		y := 0.0
		for i := 0; i < 1e3; i++ {
			A := rand.Float64() * 100
			T := rand.Float64() * 2 * math.Pi

			s += A * math.Cos(w+T)
			x += A * math.Cos(w+T)
			y += A * math.Sin(w+T)
		}
		p := Phasor{
			A: math.Hypot(x, y),
			P: f64.Rad2Deg(math.Atan2(y, x)),
			W: w,
		}
		if math.Abs(x-p.A*cosd(p.P)) > 1e-5 {
			fmt.Println("mismatched phasor: %v %v\n", x, p.A*cosd(p.P))
		}
	}
}

func cosd(d float64) float64 {
	return math.Cos(f64.Deg2Rad(d))
}

func sind(d float64) float64 {
	return math.Sin(f64.Deg2Rad(d))
}
