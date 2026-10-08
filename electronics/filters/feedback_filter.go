package main

import "fmt"

func main() {
	simfb(delta, 32, []float64{0.5})
	simfb(delta, 32, []float64{2})
}

// y = x + a1*y[t-1] + a2*y[t-2] ...
func simfb(f func(int) float64, n int, a []float64) {
	z := make([]float64, len(a))
	y := 0.0
	for i := 0; i < n; i++ {
		if len(z) > 1 {
			copy(z[1:], z)
		}
		z[0] = y

		y = f(i)
		for i := range z {
			y += a[i] * z[i]
		}
		fmt.Println(y)
	}
}

func delta(t int) float64 {
	if t == 0 {
		return 1
	}
	return 0
}
