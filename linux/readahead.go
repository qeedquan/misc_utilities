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

	for _, name := range flag.Args() {
		f, err := os.Open(name)
		if err != nil {
			fmt.Fprintln(os.Stderr, "readahead:", err)
			continue
		}

		_, _, e := syscall.Syscall(syscall.SYS_READAHEAD, f.Fd(), 0, ^uintptr(0))
		if e != 0 {
			fmt.Fprintln(os.Stderr, syscall.Errno(e))
		}

		f.Close()
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: file ...")
	os.Exit(2)
}
