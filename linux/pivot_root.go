// +build linux

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
	if flag.NArg() < 2 {
		usage()
	}
	ck(syscall.PivotRoot(flag.Arg(0), flag.Arg(1)))
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: pivot_root new-root put-old")
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		fmt.Fprintln(os.Stderr, "pivot_root:", err)
		os.Exit(1)
	}
}
