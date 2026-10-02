// https://en.wikipedia.org/wiki/Bernoulli_number
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
	for _, arg := range flag.Args() {
		p := new(big.Int)
		_, ok := p.SetString(arg, 0)
		if ok {
			fmt.Println(p, bernoulli(p).RatString())
		}
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: num ...")
	flag.PrintDefaults()
	os.Exit(2)
}

// https://en.wikipedia.org/wiki/Bernoulli_number#Explicit_definition
// computes B−n
func bernoulli(m *big.Int) *big.Rat {
	o := big.NewInt(1)
	r := big.NewRat(0, 1)
	for k := big.NewInt(0); k.Cmp(m) <= 0; k.Add(k, o) {
		for v := big.NewInt(0); v.Cmp(k) <= 0; v.Add(v, o) {
			c := big.NewInt(-1)
			c.Exp(c, v, nil)
			c1 := new(big.Rat)
			c1.SetInt(c)

			c2 := binomial(k, v)

			c.Exp(v, m, nil)
			d := big.NewInt(1)
			d.Add(k, o)
			c3 := new(big.Rat)
			c3.SetFrac(c, d)

			c1.Mul(c1, c2)
			c1.Mul(c1, c3)
			r.Add(r, c1)
		}
	}

	return r
}

func binomial(n, k *big.Int) *big.Rat {
	x := fact(n)
	y := fact(k)
	v := new(big.Int)
	v.Sub(n, k)
	z := fact(v)
	y.Mul(y, z)
	x.Quo(x, y)
	return x
}

func fact(n *big.Int) *big.Rat {
	o := big.NewInt(1)
	if n.Cmp(o) <= 0 {
		return big.NewRat(1, 1)
	}

	v := big.NewInt(1)
	for i := big.NewInt(2); i.Cmp(n) <= 0; i.Add(i, o) {
		v.Mul(v, i)
	}

	p := big.NewRat(1, 1)
	p.SetInt(v)
	return p
}
