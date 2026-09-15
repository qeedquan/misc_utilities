package main

import (
	"bytes"
	"flag"
	"fmt"
	"io"
	"log"
	"os"
	"strings"
)

func main() {
	flag.Usage = usage
	flag.Parse()

	var r io.Reader
	if flag.NArg() < 1 {
		b, err := io.ReadAll(os.Stdin)
		if err != nil {
			log.Fatal(err)
		}
		r = bytes.NewBuffer(b)
	} else {
		s := strings.Join(flag.Args(), " ")
		r = strings.NewReader(s)
	}

	for {
		var v uint64

		_, err := fmt.Fscanf(r, "%v", &v)
		if err != nil {
			break
		}

		fmt.Printf("%#x\n", v)
		for i := uint(8); i <= 64; i *= 2 {
			fmt.Printf("rev%d %#x\n", i, rev(v, i))
		}
		fmt.Printf("\n")
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: <value> ...")
	flag.PrintDefaults()
	os.Exit(2)
}

func rev(v uint64, n uint) uint64 {
	r := uint64(0)
	for i := uint(0); i < n; i++ {
		if v&(1<<(n-i-1)) != 0 {
			r |= 1 << i
		}
	}
	return r
}
