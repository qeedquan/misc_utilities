// https://en.wikipedia.org/wiki/Pascal%27s_triangle
package main

import (
	"bytes"
	"flag"
	"fmt"
	"log"
	"math/big"
	"os"
	"strconv"
)

var (
	binomial = flag.Bool("binomial", false, "divide each row by 2^n for binomial distribution values")
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("pascal_triangle: ")
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	depth, err := strconv.Atoi(flag.Arg(0))
	ck(err)
	tree := gen(depth)
	print(tree)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: pascal_triangle depth")
	flag.PrintDefaults()
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func gen(depth int) [][]*big.Int {
	p := make([][]*big.Int, depth)
	for i := range p {
		p[i] = make([]*big.Int, i+1)
	}
	for i := range p {
		for j := range p[i] {
			switch {
			case (i-1 < 0 || j-1 < 0) || (i-1 < 0 || j >= len(p[i-1])):
				p[i][j] = big.NewInt(1)
			default:
				p[i][j] = new(big.Int)
				p[i][j].Add(p[i-1][j-1], p[i-1][j])
			}
		}
	}
	return p
}

func print(p [][]*big.Int) {
	_, n := pstr(p[len(p)-1], len(p)-1)
	for i := range p {
		s, m := pstr(p[i], i)
		for j := 0; j < (n-m)/2; j++ {
			fmt.Printf(" ")
		}
		fmt.Println(s)
	}
}

func pstr(p []*big.Int, d int) (string, int) {
	b := new(bytes.Buffer)
	for i := range p {
		if *binomial {
			x := big.NewInt(2)
			y := big.NewInt(int64(d))
			x.Exp(x, y, nil)

			f := new(big.Float)
			g := new(big.Float)
			f.SetInt(p[i])
			g.SetInt(x)
			f.Quo(f, g)

			fmt.Fprintf(b, "%s", f.String())
		} else {
			fmt.Fprintf(b, "%s", p[i])
		}
		if i <= len(p)-1 {
			fmt.Fprintf(b, " ")
		}
	}
	s := b.String()
	return s, len(s)
}
