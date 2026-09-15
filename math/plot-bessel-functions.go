/*

https://www.math24.net/bessel-differential-equation/

*/

package main

import (
	"flag"
	"fmt"
	"math"
	"os"
	"strconv"
)

func main() {
	flag.Parse()
	if flag.NArg() != 4 {
		usage()
	}

	x0, _ := strconv.ParseFloat(flag.Arg(0), 64)
	x1, _ := strconv.ParseFloat(flag.Arg(1), 64)
	dx, _ := strconv.ParseFloat(flag.Arg(2), 64)
	m, _ := strconv.Atoi(flag.Arg(3))
	gen(x0, x1, dx, m)
}

/*

Bessel equation are of the type
x^2y'' + xy' + (x^2 - v^2)y = 0

Where v is the order of the solution
If v is a non-integer the solution is of the form

y(x) = C1*J(v, x) + C2*J(-v, x)

where J(v, x) are bessel functions of the first kind

J(v, x) = Sum(p, 0, oo) (-1)^p/(Gamma(p+1)*Gamma(p+v+1)) * (x/2)^(2p+v)

If v is an integer then the solution is of the form

y(x) = C1*Y(v, x) + C2*Y(v, x)

where Y(v, x) are bessel functions of the second kind

Y(v, x) = (J(v, x)*cos(pi*v) - J(-v, x)) / sin(pi*v)

These are some other forms that can be reduced to bessel function

Let x = ix
y(x) = C1*J(v, -ix) + C2*Y(v, -ix) = C3*I(v, x) + C4*K(v, x)
where I(v, x) and K(v, x) are modified bessel function of first and second kind

Airy differential equation
y'' - xy = 0
y(x) = C1*sqrt(x)*J(1/3, 2/3*i*x^(3/2)) + C2*sqrt(x)*J(-1/3, 2/3*i*x^(3/2))

x^2y'' + xy' + (ax^2 - v^2)y = 0
y(x) = C1*J(v, ax) + C2*Y(v, ax)

x^2y'' + axy' + (x^2 - v^2)y = 0
y(x) = x^((1-a)/2) * [C1*J(n,x) + C2*Y(n,x)]

*/

func gen(x0, x1, dx float64, m int) {
	for x := x0; x <= x1; x += dx {
		fmt.Printf("%f ", x)
		for n := 0; n <= m; n++ {
			fmt.Printf("%f ", math.Jn(n, x))
		}
		for n := 0; n <= m; n++ {
			fmt.Printf("%f ", math.Yn(n, x))
		}
		fmt.Printf("\n")
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: <start> <end> <step> <max_order>")
	os.Exit(2)
}
