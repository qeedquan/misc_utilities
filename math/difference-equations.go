package main

import (
	"fmt"
	"math"
)

func main() {
	testSolution(Y1, Ys1, 100)
	testSolution(X2, Xs2, 100)
	testSolution(Y3, Ys3, 100)
	testSolution(Y4, Ys4, 25)
}

func testSolution(F, Fs func(int) float64, n int) {
	for i := 0; i < n; i++ {
		fmt.Println(i, F(i), Fs(i))
	}
}

// find the solution to the difference equation
// y(n) - 0.25y(n-2) = x(n) for x(n) = u(n)
// with initial conditions y(-1) = 1 and y(-2) = 0

// from a lookup table, x(n) has a particular form of C1 where C1 is a constant
// so we have for a particular solution of y(p) = C1
// y(p) = C1 ->
// C1 - 0.25*C1 = 1 since u(n) is 1 for n >= 0
// C1 = 1/(1-0.25) = 4/3
// to find a homogenous solution set y_h(n) = z^n ->
// z^2 - 0.25 = 0 -> (z+0.5)(z-0.5) = 0
// so solution has a form of
// A1*0.5^n + A2*(-0.5)^n so solution is
// 4/3 + A1*0.5^n + A2*(-0.5)^n
// using initial condition we can get
// 4/3 - 0.5*(n+1) + 1/6*(-0.5)^n
func Y1(n int) float64 {
	if n < -2 {
		return 0
	}
	if n == -2 {
		return 0
	}
	if n == -1 {
		return 1
	}
	return 0.25*Y1(n-2) + X1(n)
}

func Ys1(n int) float64 {
	n++
	return 4.0/3 - math.Pow(0.5, float64(n+1)) + 1.0/6*math.Pow(-0.5, float64(n))
}

func X1(n int) float64 {
	return U(n)
}

// express the sequence of 1, 2, 3 for n = 0, 1, 2 and 0 for everything else
// in terms of sum of scaled and shifted steps
// x(n) = delta(n) + 2*delta(n-1) + 3*delta(n-2)
// delta(n) = u(n) - u(n-1)
// x(n) = u(n) + u(n-1) + u(n-2) - 3u(n-3)
func X2(n int) float64 {
	if n == 0 {
		return 1
	}
	if n == 1 {
		return 2
	}
	if n == 2 {
		return 3
	}
	return 0
}

func Xs2(n int) float64 {
	return U(n) + U(n-1) + U(n-2) - 3*U(n-3)
}

// y(n) = -0.5*y(n-1) + 2x(n)
// solve for close form, use close form to calculate initial conditions
// and back-sub into y(n)
func Y3(n int) float64 {
	if n < 0 {
		return 0
	}
	if n == 0 {
		return 4
	}
	return -0.5*Y3(n-1) + 2*X3(n)
}

func Ys3(n int) float64 {
	A := 4 * math.Pow(-1, float64(n))
	B := 4.0 / 3
	C := 4.0 / 3 * math.Pow(-0.5, float64(n))
	return (A + B - C) * U(n)
}

func X3(n int) float64 {
	if n > 0 && n%2 == 0 {
		return 2
	}
	return 0
}

// y(n) - 5*y(n-1) + 6y(n-2) = x(n - 1)
// find step response, ie, x(n) = u(n)
func Y4(n int) float64 {
	if n < 0 {
		return 0
	}
	if n == 0 {
		return 1
	}
	return 5*Y4(n-1) - 6*Y4(n-2) + X4(n-1)
}

func Ys4(n int) float64 {
	n++
	return (0.5 + 3.0/2*math.Pow(3, float64(n)) - 2*math.Pow(2, float64(n))) * U(n)
}

func X4(n int) float64 {
	return U(n)
}

func U(n int) float64 {
	if n < 0 {
		return 0
	}
	return 1
}

func fact(n int) int {
	if n < 0 {
		return 0
	}
	if n <= 1 {
		return 1
	}
	r := 2
	for i := 2; i <= n; i++ {
		r *= i
	}
	return r
}
