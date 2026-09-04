package main

import (
	"flag"
	"fmt"
	"io"
	"log"
	"os"
	"strconv"
	"strings"
	"text/scanner"
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("guseq: ")

	flag.Usage = usage
	flag.Parse()

	if flag.NArg() < 1 {
		guseq(os.Stdin)
	} else {
		str := strings.Join(flag.Args(), " ")
		guseq(strings.NewReader(str))
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] <num> ...")
	flag.PrintDefaults()
	os.Exit(2)
}

func guseq(r io.Reader) {
	var s scanner.Scanner
	s.Init(r)

	prev := 0.0
	for i := 1; ; i++ {
		switch ch := s.Scan(); ch {
		case scanner.EOF:
			return
		case scanner.Int:
			n, err := strconv.ParseInt(s.TokenText(), 0, 64)
			if err == nil {
				cur := float64(n)
				fmt.Printf("%d, %f, %f -> %f\n", i, prev, cur, cur-prev)
				prev = cur
			}
		case scanner.Float:
			cur, err := strconv.ParseFloat(s.TokenText(), 64)
			if err == nil {
				fmt.Printf("%d, %f, %f -> %f\n", i, prev, cur, cur-prev)
				prev = cur
			}
		}
	}
}
