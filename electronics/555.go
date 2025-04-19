package main

import (
	"fmt"
	"math"
)

func main() {
	fmt.Println(duty(1, 1, 1))
	fmt.Println(duty(1, 10, 1))
}

// https://www.toothillschool.co.uk/data/uploads/design/files/Circuit_Symbols_and_Formulea.pdf
func duty(R1, R2, C float64) (hi, lo, markspace float64) {
	hi = math.Ln2 * (R1 + R2) * C
	lo = math.Ln2 * R2 * C
	markspace = hi / lo
	return
}

func afreq(R1, R2, C float64) float64 {
	return (1 / math.Ln2) * ((R1 + 2*R2) * C)
}
