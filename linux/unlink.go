// +build unix

// ported from suckless
package main

import (
	"flag"
	"fmt"
	"os"
	"syscall"
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() != 1 {
		usage()
	}
	ck(syscall.Unlink(flag.Arg(0)))
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: file")
	flag.PrintDefaults()
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		fmt.Fprintln(os.Stderr, "unlink:", err)
		os.Exit(1)
	}
}
