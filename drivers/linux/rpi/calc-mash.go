package main

import (
	"flag"
	"fmt"
	"strconv"
)

func main() {
	flag.Parse()
	if flag.NArg() != 3 {
		dumptable()
		return
	}

	// see https://elinux.org/BCM2835_datasheet_errata#p105_table
	fs := 4096.0
	source, _ := strconv.ParseFloat(flag.Arg(0), 64)
	divi, _ := strconv.ParseFloat(flag.Arg(1), 64)
	divf, _ := strconv.ParseFloat(flag.Arg(2), 64)
	for i := 0; i < 4; i++ {
		min, avg, max := calc(i, fs, source, divi, divf)
		fmt.Printf("%.2f %.2f %.2f %.2f\n", source, min, avg, max)
	}
}

func dumptable() {
	fs := 4096.0
	pll := []float64{
		650, 400, 200,
	}
	divi := []float64{
		35, 21, 10,
	}
	divf := []float64{
		492, 854, 939,
	}

	for i := range pll {
		for j := 0; j < 4; j++ {
			min, avg, max := calc(j, fs, pll[i], divi[i], divf[i])
			fmt.Printf("%.2f %.2f %.2f %.2f\n", pll[i], min, avg, max)
		}
		fmt.Println()
	}
}

func calc(mash int, fs, source, divi, divf float64) (min, avg, max float64) {
	switch mash {
	case 0:
		max, avg, min = source/divi, source/divi, source/divi
	case 1:
		max, avg, min = source/divi, source/(divi+divf/fs), source/(divi+1)
	case 2:
		max, avg, min = source/(divi-1), source/(divi+divf/fs), source/(divi+2)
	case 3:
		max, avg, min = source/(divi-3), source/(divi+divf/fs), source/(divi+4)
	}
	return
}
