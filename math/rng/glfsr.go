// https://en.wikipedia.org/wiki/Linear-feedback_shift_register
// Galois LFSR
package main

import (
	"bufio"
	"flag"
	"fmt"
	"log"
	"math/big"
	"os"
	"strconv"
)

var (
	iterations = flag.String("i", "0", "iterations (0 for all)")
	quiet      = flag.Bool("q", false, "don't print lfsr output")
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("glfsr: ")

	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	var t []uint
	for _, s := range flag.Args()[1:] {
		n, err := strconv.ParseUint(s, 0, 64)
		if err != nil {
			log.Fatal(err)
		}
		t = append(t, uint(n))
	}

	w := bufio.NewWriter(os.Stdout)
	defer w.Flush()

	p := new(big.Int)
	p.SetString(flag.Arg(0), 0)

	fmt.Fprintf(w, "start: %v\n", p)
	fmt.Fprintf(w, "taps:  %v\n\n", t)

	l := NewGLFSR(p, t...)
	j := new(big.Int)
	i := new(big.Int)
	i.Set(one)
	j.SetString(*iterations, 0)
	for ; ; i.Add(i, one) {
		v := l.Value()
		n := l.Next()
		if !*quiet {
			fmt.Fprintf(w, "%v %v\n", i, v)
		}
		if p.Cmp(n) == 0 || n.Cmp(zero) == 0 {
			break
		}
		if j.Cmp(zero) != 0 && i.Cmp(j) > 0 {
			break
		}
	}
	fmt.Fprintf(w, "\n%v iterations\n", i)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: glfsr [options] seed taps ...")
	flag.PrintDefaults()
	os.Exit(2)
}

var (
	zero = big.NewInt(0)
	one  = big.NewInt(1)
)

type GLFSR struct {
	taps  *big.Int
	start *big.Int
	val   *big.Int
}

func NewGLFSR(seed *big.Int, taps ...uint) *GLFSR {
	l := &GLFSR{
		taps:  new(big.Int),
		start: new(big.Int),
		val:   new(big.Int),
	}

	l.start.Set(seed)
	l.val.Set(seed)

	for _, t := range taps {
		x := big.NewInt(1)
		x.Lsh(x, (t - 1))
		l.taps.Or(l.taps, x)
	}
	return l
}

func (l *GLFSR) Value() *big.Int {
	p := new(big.Int)
	p.Set(l.val)
	return p
}

func (l *GLFSR) Next() *big.Int {
	lsb := new(big.Int).Set(l.val)
	lsb.And(lsb, one)

	l.val.Rsh(l.val, 1)
	if lsb.Cmp(zero) != 0 {
		l.val.Xor(l.val, l.taps)
	}

	return l.val
}

func (l *GLFSR) String() string {
	return l.val.String()
}
