package main

import (
	"fmt"
	"math"
)

func main() {
	fmt.Println(comp1(1, -3))
	fmt.Println(comp1(5, -3))
}

// comparator is a circuit that compares two analog
// inputs at the + and - terminals, output voltage
// that is logical high if + is higher than -, low otherwise
func comp1(plus, minus float64) bool {
	return math.Abs(plus) > math.Abs(minus)
}
