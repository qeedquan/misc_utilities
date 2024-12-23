package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"time"
	"unicode/utf8"
)

func main() {
	log.SetFlags(0)
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	s := flag.Arg(0)
	r, _ := utf8.DecodeLastRuneInString(s)
	if isDigit(r) {
		s += "s"
	}

	d, err := time.ParseDuration(s)
	if err != nil {
		log.Fatal(err)
	}
	time.Sleep(d)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] duration")
	flag.PrintDefaults()
	os.Exit(1)
}

func isDigit(r rune) bool {
	return '0' <= r && r <= '9'
}
