package main

import (
	"flag"
	"fmt"
	"io"
	"os"
)

var (
	value = flag.Uint("value", 42, "frob value")

	status = 0
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		_, err := io.Copy(os.Stdout, frobber{os.Stdin, byte(*value)})
		ek(err)
	} else {
		for _, name := range flag.Args() {
			ek(frobFile(name))
		}
	}

	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: filefrob [options] [file ...]")
	flag.PrintDefaults()
	os.Exit(2)
}

func frobFile(name string) error {
	in, err := os.Open(name)
	if err != nil {
		return err
	}
	defer in.Close()

	out, err := os.OpenFile(name, os.O_WRONLY, 0644)
	if err != nil {
		return err
	}

	_, err = io.Copy(out, frobber{in, byte(*value)})
	xerr := out.Close()
	if err != nil {
		return err
	}
	return xerr
}

func ek(err error) bool {
	if err != nil {
		fmt.Fprintln(os.Stderr, "filefrob:", err)
		status = 1
		return true
	}
	return false
}

type frobber struct {
	r io.Reader
	v byte
}

func (f frobber) Read(b []byte) (n int, err error) {
	n, err = f.r.Read(b)
	for i := 0; i < n; i++ {
		b[i] ^= f.v
	}
	return
}
