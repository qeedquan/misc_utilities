// ported from suckless

package main

import (
	"flag"
	"fmt"
	"io"
	"os"
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() != 1 {
		usage()
	}

	f, err := os.CreateTemp(os.TempDir(), "sponge")
	ck(err)
	defer os.Remove(f.Name())
	defer f.Close()

	_, err = io.Copy(f, os.Stdin)
	ck(err)
	f.Seek(0, io.SeekStart)

	w, err := os.Create(flag.Arg(0))
	ck(err)
	defer w.Close()

	_, err = io.Copy(w, f)
	ck(err)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: sponge file")
	flag.PrintDefaults()
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		fmt.Fprintln(os.Stderr, "sponge:", err)
		os.Exit(1)
	}
}
