package main

import (
	"flag"
	"fmt"
	"log"
	"math/big"
	"os"
)

func main() {
	var (
		sep   string
		first *big.Rat
		inc   *big.Rat
		last  *big.Rat
	)

	log.SetFlags(0)
	log.SetPrefix("")

	flag.StringVar(&sep, "s", "\n", "separator between numbers")
	flag.Usage = usage
	flag.Parse()

	switch a := flag.Args(); len(a) {
	case 3:
		first = newrat(a[0])
		inc = newrat(a[1])
		last = newrat(a[2])
	case 2:
		first = newrat(a[0])
		inc = newrat("1")
		last = newrat(a[1])
	case 1:
		first = newrat("1")
		inc = newrat("1")
		last = newrat(a[0])
	default:
		usage()
	}

	seq(first, inc, last, sep)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: seq [options] last")
	fmt.Fprintln(os.Stderr, "   or: seq [options] first last")
	fmt.Fprintln(os.Stderr, "   or: seq [options] first increment last")
	flag.PrintDefaults()
	os.Exit(2)
}

func newrat(s string) *big.Rat {
	n, _ := new(big.Rat).SetString(s)
	if n == nil {
		log.Fatalf("invalid number %q", s)
	}
	return n
}

func seq(first, inc, last *big.Rat, sep string) {
	num := first
	for once := true; ; once = false {
		if num.Cmp(last) > 0 {
			break
		}

		if !once {
			fmt.Printf("%s", sep)
		}

		if num.IsInt() {
			fmt.Printf("%s", num.Num())
		} else {
			prec := len(inc.Denom().String()) + 1
			fmt.Printf("%s", num.FloatString(prec))
		}
		num = num.Add(num, inc)
	}
	fmt.Printf("\n")
}
