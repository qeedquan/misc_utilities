package main

import (
	"flag"
	"fmt"
	"os"
	"unicode"
)

var (
	dump = flag.Bool("d", false, "dump telepad")
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if *dump {
		printTelepad()
		return
	}

	if flag.NArg() < 1 {
		usage()
	}
	for _, arg := range flag.Args() {
		fmt.Println(tel2num(arg))
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] telepad ...")
	flag.PrintDefaults()
	os.Exit(2)
}

func printTelepad() {
	fmt.Println("ABC -> 1")
	fmt.Println("DEF -> 2")
	fmt.Println("GHI -> 3")
	fmt.Println("JKL -> 4")
	fmt.Println("MNO -> 5")
	fmt.Println("MNO -> 6")
	fmt.Println("PQRS -> 7")
	fmt.Println("TUV -> 8")
	fmt.Println("WXYZ -> 9")
}

func tel2num(s string) string {
	var p string
	for _, c := range s {
		switch unicode.ToUpper(c) {
		case 'A', 'B', 'C':
			p += "2"
		case 'D', 'E', 'F':
			p += "3"
		case 'G', 'H', 'I':
			p += "4"
		case 'J', 'K', 'L':
			p += "5"
		case 'M', 'N', 'O':
			p += "6"
		case 'P', 'Q', 'R', 'S':
			p += "7"
		case 'T', 'U', 'V':
			p += "8"
		case 'W', 'X', 'Y', 'Z':
			p += "9"
		}
	}
	return p
}
