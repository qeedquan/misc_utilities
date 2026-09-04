package main

import (
	"compress/bzip2"
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
		ek(bzcat(os.Stdin, "<stdin>"))
	} else {
		for i := 0; i < flag.NArg(); i++ {
			s := flag.Arg(i)
			f, err := os.Open(s)
			if ek(err) {
				continue
			}

			ek(bzcat(f, s))
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

func bzcat(f *os.File, s string) error {
	r := bzip2.NewReader(f)

	_, err := io.Copy(os.Stdout, r)
	if err != nil {
		return fmt.Errorf("%v: %v", s, err)
	}

	return nil
}
