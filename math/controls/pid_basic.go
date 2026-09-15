// https://blog.wesleyac.com/posts/intro-to-control-part-one-pid
// https://blog.wesleyac.com/posts/intro-to-control-part-three-simulation
package main

import "fmt"

func main() {
	var s [4]State
	for i := range s {
		s[i].Init()
		s[i].control = i
	}
	for n := 0; ; n++ {
		for i := range s {
			s[i].Update()
			fmt.Printf("%d goal % .10f x % .10f error % .10f control%d\n",
				n, s[i].goal, s[i].x, s[i].goal-s[i].x, i)
		}
		fmt.Println()
	}
}

type State struct {
	timestep float64
	goal     float64
	x        float64
	lastx    float64
	xdot     float64
	xddot    float64
	integral float64
	ff       float64
	fg       float64
	mass     float64
	control  int
}

func (s *State) Init() {
	*s = State{
		timestep: 0.05,
		goal:     5,
		x:        8,
		lastx:    8,
		xdot:     0,
		xddot:    0,
		ff:       10,
		fg:       9.8,
		mass:     100,
	}
}

func (s *State) Update() {
	// euler integration for equation of motion
	// v = a*dt
	// p = v*dt
	s.xdot += s.xddot * s.timestep
	s.x += s.xdot * s.timestep

	// at each timestep apply control which in this case
	// a force to change acceleration to tries to move x (the position)
	// to the goal
	s.applyControl()
}

func (s *State) SetForce(f float64) {
	// Use newton equation F = ma
	// a = F/m
	// gravity is a force so no conversion (divide by mass) is needed

	// f is the control force on which we apply to keep it moving to towards the goal

	// ff is the dampening force against f (its the force of the elevator)
	// this term is for making the motion differs depending how fast the elevator was moving
	// it is in the same direction as gravity (downwards) since it opposes f
	// this is the friction term

	// the negative sign represents the direction down
	s.xddot = (f / s.mass) - ((s.ff * s.xdot / s.mass) + s.fg)
}

func (s *State) applyControl() {
	var f float64
	switch s.control {
	case 0:
		f = s.control1(s.x, s.goal)
	case 1:
		f = s.control2(s.x, s.goal)
	case 2:
		f = s.control3(s.x, s.goal)
	case 3:
		f = s.control4(s.x, s.goal)
	}
	f = clamp(f, -1, 1) * 5000
	s.SetForce(f)
}

// the PID controller family works by
// setting new_state = Kp*e + Ki*integral + Kd*derivative
// and we adjust the Kp, Ki, Kd constants to get the response
// we want.

// Usually we think of integrals/derivatives as being in different
// dimensions as the original unit, if we were to sum them like this
// we need to think in a different terms to justify the operation

// Since the integral is all the error terms adding up, we can see this
// as sum over all errors and that is in the same dimension as the original units
// as E = e1 + e2 + e3 ... is in the same units each individual term

// For derivative we are setting it to be
// d = x - lastx
// so this is also fine if we view it as a negative addition of a bunch of terms
// so it can be in the same units

// Otherwise, we have to view the constant themselves have to have units that cancel out the
// integrals/derivatives units and move it back it into the original unit space

// bang-bang controller, abruptly switch states
// this is a naive method in which if you overshoot/undershoot
// the goal, just move the other direction to go back to the goal
// this leads to oscillation and the accuracy is not good at all
func (s *State) control1(x, goal float64) float64 {
	if x < goal {
		return 1
	} else if x > goal {
		return -1
	}
	return 0
}

// P (proportional) controller
// the idea behind a proportional controller is that
// as we get closer, the error term will become smaller
// it is proportional to our goal, so we take smaller steps
// as we get closer to the goal
func (s *State) control2(x, goal float64) float64 {
	e := goal - x
	P := 0.3
	return e * P
}

// PD (proportional, derivative) controller
// the derivative term anticipates the future by adjusting
// in advance the movement of it
func (s *State) control3(x, goal float64) float64 {
	P := 0.6
	D := 2.5
	e := goal - x
	d := x - s.lastx
	s.lastx = x
	return (e * P) - (d * D)
}

// PID (proportional, integral, derivative controller
// the integral term controls the error gain, as it is proportional
// to the global error, as it keep track of error history it gives more information
// than the just the P term
func (s *State) control4(x, goal float64) float64 {
	P := 0.4
	D := 6.5
	I := 0.001

	e := goal - x
	d := x - s.lastx
	i := s.integral + e

	s.lastx = x
	s.integral = i

	return (e * P) + (i * I) - (d * D)
}

func clamp(x, a, b float64) float64 {
	if x < a {
		x = a
	}
	if x > b {
		x = b
	}
	return x
}
