/*

https://en.wikipedia.org/wiki/Gamma_function
https://en.wikipedia.org/wiki/Multiplication_theorem#Gamma_function%E2%80%93Legendre_formula
https://www.youtube.com/watch?v=-HwwY4czx_E

The gamma function is a generalization of the factorial function to complex numbers.
It is one of the most common special functions that shows up in alot of contexts.

*/

package main

import (
	"fmt"
	"math"
	"math/rand"
)

func main() {
	test_gamma_factorial()
	test_reflection_formula()
	test_legendre_formula()
}

/*

For integer n:
Gamma(n) = Factorial(n)

*/

func test_gamma_factorial() {
	for i := 0.0; i <= 10; i++ {
		fmt.Println(math.Gamma(i))
	}
}

/*

Euler reflection formula
Gamma(1 - z) * Gamma(z) = pi / sin(pi*z)

*/

func test_reflection_formula() {
	for range 10 {
		z := rand.Float64() * 100
		a := math.Gamma(1-z) * math.Gamma(z)
		b := math.Pi / math.Sin(math.Pi*z)
		fmt.Println(a, b)
	}
}

/*

Legendre formula
Gamma(z) * Gamma(z + 0.5) = 2^*(1 - 2*z) * sqrt(pi)*Gamma(2*z)

*/

func test_legendre_formula() {
	for range 10 {
		z := rand.Float64()
		a := math.Gamma(z) * math.Gamma(z+0.5)
		b := math.Pow(2, 1-2*z) * math.Sqrt(math.Pi) * math.Gamma(2*z)
		fmt.Println(a, b)
	}
}
