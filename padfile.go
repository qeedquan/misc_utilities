package main

import (
	"flag"
	"fmt"
	"io"
	"log"
	"os"
)

var (
	padmul = flag.Uint64("m", 0, "pad to the multiple")
	padval = flag.Uint64("v", 0, "pad with value")
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("padfile: ")
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	input := flag.Arg(0)
	output := input
	if flag.NArg() >= 2 {
		output = flag.Arg(1)
	}

	err := padfile(input, output)
	if err != nil {
		log.Fatal(err)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: padfile [options] input [output]")
	flag.PrintDefaults()
	os.Exit(2)
}

func padfile(input, output string) (err error) {
	if input == output {
		return padfileap(input)
	}

	r, err := os.Open(input)
	if err != nil {
		return
	}
	defer r.Close()

	w, err := os.Create(output)
	if err != nil {
		return
	}
	defer func() {
		xerr := w.Close()
		if err == nil {
			err = xerr
		}
	}()

	n, err := io.Copy(w, r)
	if err != nil {
		return
	}

	err = padw(w, uint64(n))
	return
}

func padfileap(name string) (err error) {
	f, err := os.OpenFile(name, os.O_APPEND|os.O_WRONLY, 0644)
	if err != nil {
		return err
	}

	defer func() {
		xerr := f.Close()
		if err == nil {
			err = xerr
		}
	}()

	fi, err := f.Stat()
	if err != nil {
		return
	}

	err = padw(f, uint64(fi.Size()))
	return
}

func padw(w io.Writer, n uint64) error {
	if *padmul == 0 {
		return nil
	}
	b := []byte{byte(*padval)}
	m := multiple(n, *padmul)
	for ; n < m; n++ {
		_, err := w.Write(b)
		if err != nil {
			return err
		}
	}
	return nil
}

func multiple(a, m uint64) uint64 {
	return ((a + m - 1) / m) * m
}
