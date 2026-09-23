// https://en.wikipedia.org/wiki/Linear_congruential_generator
package main

import (
	"flag"
	"fmt"
	"log"
	"math/big"
	"os"
	"strings"
)

var tab = []struct {
	id      string
	m, a, c *Int
}{
	{"numerical recipes", pow2(32), xint("1664525"), xint("1013904223")},
	{"borland c/c++", pow2(32), xint("22695477"), xint("1")},
	{"glibc", pow2m1(31), xint("1103515245"), xint("12345")},
	{"c standard", pow2(31), xint("1103515245"), xint("12345")},
	{"borland pascal", pow2(32), xint("134775813"), xint("1")},
	{"turbo pascal", pow2(32), xint("134775813"), xint("1")},
	{"microsoft visual/quick c/c++", pow2(32), xint("214013"), xint("2531011")},
	{"microsoft visual basic 6", pow2(24), xint("1140671485"), xint("12820163")},
	{"rtluniform", pow2m1(31), xint("2147483629"), xint("2147483587")},
	{"apple carbonlib/c++11 minstd_rand0", pow2m1(31), xint("16807"), xint("0")},
	{"c++11 minstd_rand", pow2m1(31), xint("48271"), xint("0")},
	{"mmix", pow2(64), xint("6364136223846793005"), xint("1442695040888963407")},
	{"musl", pow2(64), xint("6364136223846793005"), xint("1")},
	{"vms", pow2(32), xint("69069"), xint("1")},
	{"java", pow2(48), xint("25214903917"), xint("11")},
	{"random0", xint("134456"), xint("8121"), xint("28411")},
	{"posix rand48", pow2(48), xint("25214903917"), xint("11")},
	{"cc65", pow2(32), xint("16843009"), xint("826366247")},
}

func main() {
	log.SetFlags(0)
	log.SetPrefix("lcg: ")

	var (
		M, A, C = find("musl")
		S       = xint("1")
		L, H    string
		T       string
	)
	flag.Usage = usage
	flag.Var(M, "m", "modulus")
	flag.Var(A, "a", "multiplier")
	flag.Var(C, "c", "increment")
	flag.Var(S, "s", "seed")
	flag.StringVar(&L, "l", "", "low range value")
	flag.StringVar(&H, "h", "", "high range value")
	flag.StringVar(&T, "t", "", "use pre-define type")
	flag.Parse()

	if T != "" {
		M, A, C = find(T)
	}

	lcg := newlcg(S, M, A, C, xint(L), xint(H))
	for {
		lcg.Next()
		fmt.Println(lcg.Value())
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: lcg [options]")
	flag.PrintDefaults()
	fmt.Fprintf(os.Stderr, "\nvalid types (type, modulus, multiplier, increment):\n")
	for _, p := range tab {
		fmt.Fprintf(os.Stderr, "%-48s %v %v %v\n", p.id, p.m, p.a, p.c)
	}
	os.Exit(2)
}

func find(s string) (m, a, c *Int) {
	n := 0
	for _, p := range tab {
		if strings.HasPrefix(p.id, s) {
			m, a, c = dup(p.m), dup(p.a), dup(p.c)
			n++
		}
	}
	if n != 1 {
		log.Fatalf("failed to find unique match for %q", s)
	}
	return
}

func pow2(v uint) *Int {
	p := xint("1")
	p.Lsh(p.Int, v)
	return p
}

func pow2m1(v uint) *Int {
	p := pow2(v)
	p.Sub(p.Int, big.NewInt(1))
	return p
}

func xint(s string) *Int {
	if s == "" {
		return nil
	}
	p := &Int{new(big.Int)}
	p.SetString(s, 0)
	return p
}

func dup(s *Int) *Int {
	return xint(s.String())
}

type Int struct {
	*big.Int
}

func (p Int) String() string {
	return p.Int.String()
}

func (p *Int) Set(s string) error {
	*p = Int{new(big.Int)}
	_, ok := p.SetString(s, 0)
	if !ok {
		return fmt.Errorf("invalid value: %q", s)
	}
	return nil
}

func (p *Int) Add(x, y *Int) *Int {
	p.Int = p.Int.Add(x.Int, y.Int)
	return p
}

func (p *Int) Mul(x, y *Int) *Int {
	p.Int = p.Int.Mul(x.Int, y.Int)
	return p
}

func (p *Int) Mod(x, y *Int) *Int {
	p.Int = p.Int.Mod(x.Int, y.Int)
	return p
}

type LCG struct {
	m, a, c *Int
	l, h    *Int
	s, v, z *Int
}

func newlcg(s, m, a, c, l, h *Int) *LCG {
	return &LCG{
		m: m,
		a: a,
		c: c,
		l: l,
		h: h,
		s: s,
		v: dup(s),
		z: dup(s),
	}
}

func (p *LCG) Reset() {
	p.v = dup(p.s)
	p.z = dup(p.v)
}

func (p *LCG) Value() *Int {
	return p.z
}

func (p *LCG) Next() *Int {
	p.v.Mul(p.v, p.a)
	p.v.Add(p.v, p.c)
	p.v.Mod(p.v, p.m)
	if p.l != nil && p.h != nil {
		o := big.NewRat(1, 1)
		m := new(big.Rat)
		l := new(big.Rat)
		h := new(big.Rat)
		m.SetInt(p.m.Int)
		l.SetInt(p.l.Int)
		h.SetInt(p.h.Int)
		u := new(big.Rat)
		u.Add(u, h)
		u.Sub(u, l)
		u.Add(u, o)

		v := new(big.Rat)
		v.SetInt(p.v.Int)
		v.Quo(v, m)
		v.Mul(v, u)
		v.Add(v, l)

		s := v.FloatString(1)
		n := strings.Index(s, ".")
		if n >= 0 {
			s = s[:n]
		}
		p.z = xint(s)
	} else {
		p.z = dup(p.v)
	}
	return p.z
}
