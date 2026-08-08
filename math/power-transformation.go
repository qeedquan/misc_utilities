/*

https://www.stat.umn.edu/arc/yjpower.pdf
https://en.wikipedia.org/wiki/Power_transform

*/

package main

import (
	"fmt"
	"math"
	"math/rand"
)

func main() {
	for i := 0; i < 1000; i++ {
		y := rand.Float64()
		fmt.Printf("%.6f ", y)
		for j := 0.0; j <= 10; j++ {
			fmt.Printf("%.6f ", yj(j, y))
		}
		fmt.Println()
	}
}

func yj(l, y float64) float64 {
	switch {
	case l != 0 && y >= 0:
		return (math.Pow(y+1, l) - 1) / l
	case l == 0 && y >= 0:
		return math.Log(y + 1)
	case l != 2 && y < 0:
		return -(math.Pow(-y+1, 2-l) - 1) / (2 - l)
	case l == 2 && y < 0:
		return -math.Log(-y + 1)
	}
	return 0
}
