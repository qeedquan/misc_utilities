// +build linux

// ported from suckless
package main

import (
	"flag"
	"fmt"
	"os"
	"syscall"
)

var (
	size   = flag.Int64("l", 0, "specify size")
	offset = flag.Int64("o", 0, "specify offset")

	status = 0
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	status := 0
	for _, name := range flag.Args() {
		f, err := os.OpenFile(name, os.O_RDWR|os.O_CREATE, 0644)
		if ek(err) {
			continue
		}
		ek(syscall.Fallocate(int(f.Fd()), 0, *offset, *size))
	}
	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: fallocate [-lo] file ...")
	flag.PrintDefaults()
	os.Exit(2)
}

func ek(err error) bool {
	if err != nil {
		fmt.Fprintln(os.Stderr, "fallocate:", err)
		status |= 1
		return true
	}
	return false
}
