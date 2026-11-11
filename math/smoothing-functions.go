// https://en.wikipedia.org/wiki/Smoothstep

/*

N = system("awk 'NR==1{print NF}' data.plt")
plot for [i=1:N] "data.plt" u 0:i w l title "Column ".i

*/

package main

import (
	"fmt"
	"math"
)

func main() {
	for x := 0.0; x <= 1.0; x += 0.001 {
		fmt.Printf("%.3f %.6f %.6f %.6f %.6f %.6f %.6f %.6f %.6f\n",
			x,
			softclip(x),
			smoothstep0(x),
			smoothstep1(x),
			smoothstep2(x),
			smoothstep3(x),
			smoothstep4(x),
			smoothstep5(x),
			smoothstep6(x),
		)
	}
}

func softclip(x float64) float64 {
	return math.Sqrt(2) * x / math.Sqrt(1+x*x)
}

func smoothstep0(x float64) float64 {
	return x
}

func smoothstep1(x float64) float64 {
	return -2*x*x*x + 3*x*x
}

func smoothstep2(x float64) float64 {
	return 6*x*x*x*x*x - 15*x*x*x*x + 10*x*x*x
}

func smoothstep3(x float64) float64 {
	return -20*x*x*x*x*x*x*x + 70*x*x*x*x*x*x - 84*x*x*x*x*x + 35*x*x*x*x
}

func smoothstep4(x float64) float64 {
	return 70*x*x*x*x*x*x*x*x*x -
		315*x*x*x*x*x*x*x*x +
		540*x*x*x*x*x*x*x -
		420*x*x*x*x*x*x +
		126*x*x*x*x*x
}

func smoothstep5(x float64) float64 {
	return -252*x*x*x*x*x*x*x*x*x*x*x +
		1386*x*x*x*x*x*x*x*x*x*x -
		3080*x*x*x*x*x*x*x*x*x +
		3465*x*x*x*x*x*x*x*x -
		1980*x*x*x*x*x*x*x +
		462*x*x*x*x*x*x
}

func smoothstep6(x float64) float64 {
	return 924*x*x*x*x*x*x*x*x*x*x*x*x*x -
		6006*x*x*x*x*x*x*x*x*x*x*x*x +
		16380*x*x*x*x*x*x*x*x*x*x*x -
		24024*x*x*x*x*x*x*x*x*x*x +
		20020*x*x*x*x*x*x*x*x*x -
		9009*x*x*x*x*x*x*x*x +
		1716*x*x*x*x*x*x*x
}
