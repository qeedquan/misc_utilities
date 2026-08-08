// ported from 9front read
package main

import (
	"flag"
	"fmt"
	"io"
	"os"
)

var (
	multi  = flag.Bool("m", false, "read all lines (in line mode)")
	nlines = flag.Int("n", 0, "read up to n lines")
	nchars = flag.Int("c", 0, "read up to n bytes")

	status int
)

func main() {
	flag.Usage = usage
	flag.Parse()

	proc := lines
	flag.Visit(func(f *flag.Flag) {
		if f.Name == "c" {
			proc = chars
		}
	})

	if flag.NArg() == 0 {
		proc(os.Stdin, "<stdin>")
	} else {
		for _, name := range flag.Args() {
			f, err := os.Open(name)
			if err != nil {
				fmt.Fprintln(os.Stderr, "read:", err)
				os.Exit(2)
			}
			proc(f, name)
			f.Close()
		}
	}

	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] [files...]")
	flag.PrintDefaults()
	os.Exit(4)
}

func line(r io.Reader, name string) int {
	var c [1]byte
	var p []byte

	for {
		n, err := r.Read(c[:])

		if err != nil && err != io.EOF {
			fmt.Fprintln(os.Stderr, "read:", err)
			os.Exit(3)
		}

		if n > 0 {
			p = append(p, c[0])
		}

		if err == io.EOF {
			if len(p) == 0 {
				status = 1
			}
			break
		}

		if c[0] == '\n' {
			break
		}
	}

	os.Stdout.Write(p)
	return len(p)
}

func lines(r io.Reader, name string) {
	for {
		if line(r, name) == 0 {
			break
		}

		if *multi {
			continue
		}

		if *nlines--; *nlines > 0 {
			continue
		}

		break
	}
}

func chars(r io.Reader, name string) {
	var p [8192]byte
	var n int

	for m := 0; m < *nchars; m += n {
		n = len(p)
		if n > *nchars-m {
			n = *nchars - m
		}

		var err error
		n, err = r.Read(p[:n])

		if err != nil && err != io.EOF {
			fmt.Fprintln(os.Stderr, "read:", err)
			os.Exit(3)
		}

		if n > 0 {
			os.Stdout.Write(p[:n])
		}

		if err == io.EOF {
			if m == 0 {
				status = 1
			}
			break
		}
	}
}
