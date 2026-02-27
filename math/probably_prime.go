package main

import (
	"flag"
	"fmt"
	"math/big"
	"os"
)

var (
	times = flag.Int("n", 100, "n iterations of random testing")
)

func main() {
	flag.Parse()

	for _, arg := range flag.Args() {
		n := new(big.Int)
		_, ok := n.SetString(arg, 0)
		if !ok {
			fmt.Fprintf(os.Stderr, "%q is an invalid number!\n", arg)
			continue
		}
		fmt.Printf("%v: %v\n", n, n.ProbablyPrime(*times))
	}
}
