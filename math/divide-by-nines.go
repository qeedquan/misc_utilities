package main

import (
	"fmt"
	"log"
	"math/big"
	"strings"
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("divide-by-nines: ")
	for i := int64(0); i < 1e4; i++ {
		_, period := div9(big.NewInt(i), 1e3)
		fmt.Println(i, period)
	}
}

func newint(s string) *big.Int {
	x := new(big.Int)
	_, ok := x.SetString(s, 0)
	if !ok {
		log.Fatal("invalid integer:", s)
	}
	return x
}

// if we divide by the number x by 9.999....
// where 9.999... is as long as the number of digits x has
// it generates a repeating pattern of that digit until it
// dries out, we log that inside period
// if the number happens to be 99 etc then it will not work
// and it won't repeat
func div9(x *big.Int, prec int) (val string, period int) {
	n := new(big.Rat)
	d := new(big.Rat)
	n.SetInt(x)

	t := x.String()
	s := ""
	for i := 0; i < len(t); i++ {
		s += "9"
		if i == 0 {
			s += "."
		}
	}
	if len(t) == 1 {
		s = ".9"
	}
	d.SetString(s)
	n.Quo(n, d)

	f := new(big.Float)
	f.SetRat(n)

	val = f.Text('f', prec)
	val = strings.ReplaceAll(val, ".", "")

loop:
	for j := 0; ; period++ {
		for i := 0; i < len(t); i, j = i+1, j+1 {
			if j >= len(val) || t[i] != val[j] {
				break loop
			}
		}
	}
	return
}
