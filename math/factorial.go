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

	fact(flag.Arg(0))
}

func fact(s string) {
	n := big.NewInt(0)
	n.SetString(s, 0)

	o := big.NewInt(1)
	r := big.NewInt(1)
	for i := big.NewInt(2); i.Cmp(n) <= 0; i.Add(i, o) {
		r.Mul(r, i)
	}
	fmt.Println(r)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: factorial num")
	flag.PrintDefaults()
	os.Exit(2)
}
