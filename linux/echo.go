package main

import (
	"flag"
	"fmt"
	"os"
)

func main() {
	nflag := flag.Bool("n", false, "do not output trailing newline")
	flag.Usage = usage
	flag.Parse()

	args := flag.Args()
	for i, s := range args {
		fmt.Print(s)
		if i+1 < len(args) {
			fmt.Print(" ")
		}
	}

	if !*nflag {
		fmt.Println()
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] string ...")
	flag.PrintDefaults()
	os.Exit(1)
}
