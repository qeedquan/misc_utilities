package main

import (
	"fmt"
	"math/big"
)

func main() {
	a := []string{"abc", "def", "ghi", "jkl", "2", "33", "44"}
	powerset(a, printset)

	b := []string{"x"}
	powerset(b, printset)
}

func printset(p []string) {
	fmt.Println(p)
}

func powerset(v []string, f func([]string)) {
	o := big.NewInt(1)
	n := big.NewInt(1)
	n.Lsh(n, uint(len(v)))
	for i := big.NewInt(0); i.Cmp(n) < 0; i.Add(i, o) {
		var p []string
		for j := 0; j < n.BitLen(); j++ {
			if i.Bit(j) != 0 {
				p = append(p, v[j])
			}
		}
		f(p)
	}
}
