package main

import (
	"flag"
	"fmt"
	"os"
	"path/filepath"
	"strings"
	"unicode"
)

var (
	status = 0
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	for _, name := range flag.Args() {
		dir, base := filepath.Split(name)
		if caps(base) {
			base = strings.ToLower(base)
			ek(os.Rename(name, filepath.Join(dir, base)))
		}
	}
	os.Exit(status)
}

func ek(err error) {
	if err != nil {
		fmt.Fprintln(os.Stderr, "nocaps:", err)
		status |= 1
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: file ...")
	os.Exit(1)
}

func caps(s string) bool {
	caps := false
	for _, r := range s {
		if unicode.IsLower(r) {
			return false
		}
		if unicode.IsUpper(r) {
			caps = true
		}
	}

	return caps
}
