// ported from unix v10

package main

import (
	"flag"
	"fmt"
	"io"
	"log"
	"math"
	"os"
	"strconv"
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("zero: ")

	flag.Usage = usage
	flag.Parse()
	if flag.NArg() > 2 {
		usage()
	}

	r := &reader{0, math.MaxUint64}
	if flag.NArg() > 0 {
		r.b, _ = strconv.Atoi(flag.Arg(0))
	}
	if flag.NArg() > 1 {
		r.n, _ = strconv.ParseUint(flag.Arg(1), 0, 64)
	}
	_, err := io.Copy(os.Stdout, r)
	if err != nil {
		log.Fatal(err)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: zero [byte [nbytes]]")
	flag.PrintDefaults()
	os.Exit(2)
}

type reader struct {
	b int
	n uint64
}

func (r *reader) Read(b []byte) (int, error) {
	if r.n == 0 {
		return 0, io.EOF
	}

	n := uint64(len(b))
	if n > r.n {
		n = r.n
	}
	b = b[:n]
	for i := range b {
		b[i] = byte(r.b)
	}
	r.n -= n

	return int(n), nil
}
