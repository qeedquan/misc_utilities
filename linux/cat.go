package main

import (
	"flag"
	"fmt"
	"io"
	"os"
)

func main() {
	flag.Parse()

	if flag.NArg() < 1 {
		cat(os.Stdin, "<stdin>")
	} else {
		for i := 0; i < flag.NArg(); i++ {
			s := flag.Arg(i)
			f, err := os.Open(s)
			if err != nil {
				fmt.Fprintln(os.Stderr, err)
				continue
			}

			cat(f, s)
			f.Close()
		}
	}
}

func cat(f *os.File, s string) {
	_, err := io.Copy(os.Stdout, f)
	if err != nil && err != io.EOF {
		fmt.Fprintf(os.Stderr, "%v: %v\n", s, err)
	}
}
