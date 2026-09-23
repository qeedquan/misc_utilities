package main

import (
	"flag"
	"fmt"
	"strconv"
	"unicode"
)

var (
	nflag = flag.Bool("n", false, "show only numbers")
	pflag = flag.Bool("p", true, "show only printables")
)

func main() {
	start, end := rune(0), unicode.MaxRune
	flag.Parse()
	switch flag.NArg() {
	case 0:
	case 1:
		start = ator(flag.Arg(0))
		end = start
	default:
		start = ator(flag.Arg(0))
		end = ator(flag.Arg(1))
	}
	for i := start; i <= end; i++ {
		if filter(i) {
			fmt.Printf("%x: %c\n", i, i)
		}
	}
}

func filter(c rune) bool {
	if *pflag && !unicode.IsPrint(c) {
		return false
	}
	if *nflag && !unicode.IsNumber(c) {
		return false
	}
	return true
}

func ator(s string) rune {
	n, _ := strconv.Atoi(s)
	return rune(n)
}
