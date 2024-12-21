package main

import (
	"flag"
	"fmt"
	"log"
	"math/big"
	"os"
)

func main() {
	log.SetPrefix("round: ")
	log.SetFlags(0)

	flag.Bool("d", false, "round down")
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() != 2 {
		usage()
	}

	mode := 'u'
	flag.Visit(func(f *flag.Flag) {
		switch f.Name {
		case "d":
			mode = 'd'
		}
	})

	target := mknum(flag.Arg(1))
	num := mknum(flag.Arg(0))
	round(target, num, mode)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: round [options] number target")
	flag.PrintDefaults()
	os.Exit(2)
}

func mknum(s string) *big.Int {
	n := new(big.Int)
	_, ok := n.SetString(s, 0)
	if !ok {
		log.Fatalf("invalid number %q", s)
	}
	return n
}

func round(target, num *big.Int, mode rune) {
	zero := new(big.Int)
	if target.Cmp(zero) == 0 && num.Cmp(zero) == 0 {
		fmt.Println("0")
		return
	}

	if num.Cmp(zero) == 0 {
		log.Fatalf("number cannot be zero")
	}

	if target.Cmp(zero) < 0 || num.Cmp(zero) < 0 {
		log.Fatalf("target/number cannot be negative")
	}

	prev := new(big.Int).Set(num)
	cur := new(big.Int).Set(num)
	for cur.Cmp(target) < 0 {
		prev = new(big.Int).Set(num)
		cur.Add(cur, num)
	}

	if mode == 'd' {
		fmt.Println("%d | %#x\n", prev, prev)
	} else {
		fmt.Printf("%d | %#x\n", cur, cur)
	}
}
