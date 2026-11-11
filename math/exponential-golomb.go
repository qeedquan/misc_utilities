// https://en.wikipedia.org/wiki/Exponential-Golomb_coding
package main

import (
	"flag"
	"fmt"
	"math/big"
	"os"
)

func main() {
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	x := new(big.Int)
	y := new(big.Int)
	x.SetString(flag.Arg(0), 0)
	y.Set(x)
	if flag.NArg() > 1 {
		y.SetString(flag.Arg(1), 0)
	}
	if x.Cmp(y) > 0 {
		x, y = y, x
	}

	one := big.NewInt(1)
	for x.Cmp(y) <= 0 {
		fmt.Println(x, expg(x))
		x.Add(x, one)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: start [end]")
	flag.PrintDefaults()
	os.Exit(2)
}

func expg(x *big.Int) string {
	z := big.NewInt(1)
	z.Add(z, x)
	p := z.Text(2)
	n := z.BitLen()
	for i := 0; i < n-1; i++ {
		p = "0" + p
	}
	return p
}
