// given a known list of numbers, try to match a hash function that perfectly hashes all the input with minimal table size
package main

import (
	"bufio"
	"flag"
	"fmt"
	"io"
	"log"
	"math"
	"os"
	"strings"
)

var flags struct {
	start uint64
	end   uint64
	prime bool
	all   bool
}

func main() {
	log.SetFlags(0)
	log.SetPrefix("find-perfect-inthash: ")
	parseflags()
	if flag.NArg() < 1 {
		ck(findhash(os.Stdin, flags.start, flags.end, flags.all, flags.prime))
	} else {
		for _, name := range flag.Args() {
			f, err := os.Open(name)
			ck(err)

			ck(findhash(f, flags.start, flags.end, flags.all, flags.prime))
		}
	}
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func findhash(r io.Reader, s0, s1 uint64, all, prime bool) error {
	p, err := getints(r)
	if err != nil {
		return err
	}
	if n := uint64(len(p)); n > s0 {
		s0 = n
	}

	for s := s0; s <= s1; s++ {
		if prime && !isprime(s) {
			continue
		}

		c := 0
	loop:
		for _, h := range htab {
			m := make(map[uint64]int)
			for _, v := range p {
				u := uint64(v)
				i := h.fn(u) % s
				_, f := m[i]
				if !f {
					m[i] = v
				}
				if m[i] != v {
					continue loop
				}
			}

			fmt.Printf("hash: %s size: %d\n", h.name, s)
			fmt.Println(m)
			fmt.Println()
			c++
		}
		if (all && len(htab) == c) || (!all && c > 0) {
			return nil
		}
		if c > 0 {
			fmt.Println(strings.Repeat("-", 50))
		}
	}

	fmt.Println("no hash function found")
	return nil
}

func getints(r io.Reader) ([]int, error) {
	var (
		p []int
		v int
	)

	b := bufio.NewReader(r)
	for {
		n, err := fmt.Fscan(b, &v)
		if n != 1 || err == io.EOF {
			break
		}
		if err != nil {
			return p, err
		}
		p = append(p, v)
	}
	return p, nil
}

func parseflags() {
	flag.Usage = usage
	flags.start = 32
	flags.end = math.MaxUint64 - 1
	flags.prime = false
	flags.all = false

	ranges := fmt.Sprintf("%d %d", flags.start, flags.end)
	flag.BoolVar(&flags.all, "a", flags.all, "must match all hash function before finishing")
	flag.StringVar(&ranges, "r", ranges, "specify range of valid table sizes")
	flag.BoolVar(&flags.prime, "p", flags.prime, "only use prime table size")
	flag.Parse()

	fmt.Sscan(ranges, &flags.start, &flags.end)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] file ...")
	flag.PrintDefaults()
	os.Exit(2)
}

func isprime(n uint64) bool {
	if n < 2 {
		return false
	}

	l := math.Sqrt(float64(n))
	for i := uint64(2); i <= uint64(l); i++ {
		if n%i == 0 {
			return false
		}
	}
	return true
}

func identity(x uint64) uint64 {
	return x
}

// https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-x
func knuthmul(x uint64) uint64 {
	return (x * 2654435761) & 0xffffffff
}

func mueller(x uint64) uint64 {
	x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9
	x = (x ^ (x >> 27)) * 0x94d049bb133111eb
	x = x ^ (x >> 31)
	return x
}

func xorshift64(x uint64, i uint) uint64 {
	return x ^ (x >> i)
}

func xorhash(x uint64) uint64 {
	p := uint64(0x5555555555555555)
	c := uint64(17316035218449499591)
	return c * xorshift64(p*xorshift64(x, 32), 32)
}

func m3(x uint64) uint64 {
	x ^= (x << 13)
	x ^= (x >> 17)
	x ^= (x << 5)
	return x
}

// https://gist.github.com/badboy/6267743
func hash32shift(x uint64) uint64 {
	x = ^x + (x << 15) // x = (x << 15) - x - 1;
	x = x ^ (x >> 12)
	x = x + (x << 2)
	x = x ^ (x >> 4)
	x = x * 2057 // x = (x + (x << 3)) + (x << 11);
	x = x ^ (x >> 16)
	return x
}

func jenkins32(x uint64) uint64 {
	x = (x + 0x7ed55d16) + (x << 12)
	x = (x ^ 0xc761c23c) ^ (x >> 19)
	x = (x + 0x165667b1) + (x << 5)
	x = (x + 0xd3a2646c) ^ (x << 9)
	x = (x + 0xfd7046c5) + (x << 3)
	x = (x ^ 0xb55a4f09) ^ (x >> 16)
	return x
}

var htab = []struct {
	name string
	fn   func(uint64) uint64
}{
	{"identity", identity},
	{"knuth", knuthmul},
	{"mueller", mueller},
	{"xorhash", xorhash},
	{"m3", m3},
	{"hash32shift", hash32shift},
	{"jenkins32", jenkins32},
}
