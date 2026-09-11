package main

import (
	"flag"
	"fmt"
	"io"
	"log"
	"os"
)

var (
	padmul  = flag.Uint64("m", 0, "pad to the multiple")
	padsize = flag.Uint64("s", 0, "pad to size")
	padval  = flag.Uint64("v", 0, "pad with value")
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
	m := *padsize
	if m == 0 && *padmul != 0 {
		m = multiple(n, *padmul)
	}
	if m == 0 {
		return nil
	}

	b := make([]byte, 512*1024)
	for i := range b {
		b[i] = byte(*padval)
	}
	for n < m {
		l := min(m-n, uint64(len(b)))
		_, err := w.Write(b[:l])
		if err != nil {
			return err
		}
		n += l
	}
	return nil
}

func multiple(a, m uint64) uint64 {
	return ((a + m - 1) / m) * m
}
