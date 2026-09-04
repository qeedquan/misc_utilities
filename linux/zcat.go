package main

import (
	"compress/gzip"
	"flag"
	"fmt"
	"io"
	"os"
)

var (
	status int
)

func main() {
	flag.Parse()

	if flag.NArg() < 1 {
		ek(zcat(os.Stdin, "<stdin>"))
	} else {
		for i := 0; i < flag.NArg(); i++ {
			s := flag.Arg(i)
			f, err := os.Open(s)
			if ek(err) {
				continue
			}

			ek(zcat(f, s))
			f.Close()
		}
	}

	os.Exit(status)
}

func ek(err error) bool {
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		status = 1
		return true
	}
	return false
}

func zcat(f *os.File, s string) error {
	r, err := gzip.NewReader(f)
	if err != nil {
		return fmt.Errorf("%v: %v", s, err)
	}

	_, err = io.Copy(os.Stdout, r)
	if err != nil {
		return fmt.Errorf("%v: %v", s, err)
	}

	return nil
}
