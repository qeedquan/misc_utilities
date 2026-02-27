package main

import (
	"bufio"
	"bytes"
	"flag"
	"fmt"
	"io"
	"os"
	"unicode/utf8"
)

var (
	status = 0
)

func main() {
	flag.Usage = usage
	flag.Parse()

	if flag.NArg() < 1 {
		err := rev(os.Stdin)
		ck(err)
	} else {
		for _, name := range flag.Args() {
			f, err := os.Open(name)
			if ck(err) {
				continue
			}

			err = rev(f)
			ck(err)

			f.Close()
		}
	}
	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [file ...]")
	os.Exit(1)
}

func ck(err error) bool {
	if err != nil {
		fmt.Fprintln(os.Stderr, "rev:", err)
		status = 1
		return true
	}
	return false
}

func rev(f *os.File) error {
	r := bufio.NewReader(f)
	for {
		line, err := fgetln(r)
		for len(line) > 0 {
			r, size := utf8.DecodeLastRuneInString(line)
			if size == 0 {
				break
			}
			fmt.Printf("%c", r)
			line = line[:len(line)-size]
		}

		if len(line) > 0 || (len(line) == 0 && err == nil) {
			fmt.Println()
		}

		if err == io.EOF {
			return nil
		}
		if err != nil {
			return err
		}
	}
}

func fgetln(r *bufio.Reader) (string, error) {
	b := new(bytes.Buffer)
	for {
		c, _, err := r.ReadRune()
		if err != nil || c == '\n' {
			return b.String(), err
		}

		b.WriteRune(c)
	}
}
