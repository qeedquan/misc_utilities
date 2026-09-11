package main

import (
	"flag"
	"fmt"
	"os"
	"path/filepath"
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	for _, name := range flag.Args() {
		fmt.Println(filepath.Dir(name))
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: name ...")
	flag.PrintDefaults()
	os.Exit(1)
}
