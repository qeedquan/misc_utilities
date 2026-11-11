// https://en.wikipedia.org/wiki/Catalan_number

package main

import (
	"flag"
	"fmt"
	"math/big"
	"os"
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	n := big.NewInt(0)
	n.SetString(flag.Arg(0), 0)
	o := big.NewInt(0)
	if n.Cmp(o) < 0 {
		fmt.Fprintln(os.Stderr, "catalan: invalid number")
		os.Exit(1)
	}
	fmt.Printf("%v: %v\n", n, catalan(n))
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: catalan <num>")
	flag.PrintDefaults()
	os.Exit(2)
}

func fact(n *big.Int) *big.Int {
	x := big.NewInt(1)
	o := big.NewInt(1)
	for i := big.NewInt(1); i.Cmp(n) <= 0; i.Add(i, o) {
		x.Mul(x, i)
	}
	return x
}

func catalan(n *big.Int) *big.Int {
	x := big.NewInt(2)
	x.Mul(x, n)
	x = fact(x)

	y := big.NewInt(1)
	y.Add(y, n)

	z := fact(n)
	y.Mul(y, z)
	y.Mul(y, z)

	return x.Quo(x, y)
}
