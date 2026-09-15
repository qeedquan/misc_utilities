// https://en.wikipedia.org/wiki/NMEA_0183
package main

import (
	"bufio"
	"flag"
	"fmt"
	"io"
	"os"
	"strings"
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		nmea(os.Stdin)
	} else {
		for _, name := range flag.Args() {
			f, err := os.Open(name)
			if err != nil {
				fmt.Fprintln(os.Stderr, err)
				continue
			}
			nmea(f)
			f.Close()
		}
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] <file> ...")
	flag.PrintDefaults()
	os.Exit(2)
}

func nmea(r io.Reader) {
	s := bufio.NewScanner(r)
	for s.Scan() {
		ln := trim(s.Text())
		fmt.Printf("$%s*%02x\n", ln, xor(ln))
	}
}

func trim(s string) string {
	i := strings.IndexRune(s, '$')
	j := strings.IndexRune(s, '*')
	if i < 0 {
		i = 0
	}
	if j < 0 {
		j = len(s)
	}
	return s[i:j]
}

func xor(s string) int {
	var v int
	for _, r := range s {
		v ^= int(r)
	}
	return v
}
