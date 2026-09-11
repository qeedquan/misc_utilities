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
	if flag.NArg() < 2 {
		usage()
	}
	num := atoi(flag.Arg(0))
	degree := atoi(flag.Arg(1))
	fmt.Println(sum(num, degree))
	fmt.Println(bsum(num, degree))
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: sums_of_power num degree")
	flag.PrintDefaults()
	os.Exit(2)
}

func atoi(s string) *big.Int {
	n := new(big.Int)
	n.SetString(s, 10)
	return n
}

func sum(num, degree *big.Int) *big.Int {
	one := big.NewInt(1)
	res := big.NewInt(0)
	for i := big.NewInt(0); i.Cmp(num) <= 0; i.Add(i, one) {
		n := new(big.Int)
		n.Exp(i, degree, nil)
		res.Add(res, n)
	}
	return res
}

// bernoulli summation formula
func bsum(num, degree *big.Int) *big.Int {
	if degree.Sign() < 0 {
		return nil
	}

	o := big.NewInt(1)

	m := new(big.Int)
	m.Set(degree)
	m.Add(m, o)

	g := big.NewRat(1, 1)
	s := new(big.Rat)
	k := -1
	for i := big.NewInt(0); i.Cmp(degree) <= 0; i.Add(i, o) {
		if k++; k > 4 {
			k = 3
		}
		if k >= 3 && k&1 != 0 {
			g.Neg(g)
			continue
		}
		a := bernoulli(i)

		x := new(big.Int)
		y := new(big.Int)
		x.Set(num)
		y.Set(m)
		y.Sub(y, i)
		x.Exp(x, y, nil)
		b := new(big.Rat)
		b.SetInt(x)

		c := binomial(m, i)

		a.Mul(a, b)
		a.Mul(a, c)
		a.Mul(a, g)

		s.Add(s, a)
		g.Neg(g)
	}

	x := new(big.Rat)
	x.SetInt(m)
	x.Inv(x)
	s.Mul(s, x)

	return s.Num()
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
