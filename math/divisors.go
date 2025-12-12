package main

import (
	"flag"
	"fmt"
	"os"
	"strconv"
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	for _, args := range flag.Args() {
		v, err := strconv.ParseUint(args, 0, 64)
		if err != nil {
			fmt.Fprintf(os.Stderr, "%v: %v\n", args, err)
			continue
		}
		divisors(v)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: value ...")
	flag.PrintDefaults()
	os.Exit(2)
}

func divisors(x uint64) {
	fmt.Printf("%d |\n", x)
	for i := uint64(2); i < x; i++ {
		if x%i == 0 {
			fmt.Println(i)
		}
	}
	fmt.Println()
}
