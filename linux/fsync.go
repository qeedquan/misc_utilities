// +build linux

// ported from toybox
package main

import (
	"flag"
	"fmt"
	"os"
	"syscall"
)

var (
	dflag = flag.Bool("d", false, "avoid syncing metadata")

	status = 0
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	for _, name := range flag.Args() {
		f, err := os.OpenFile(name, syscall.O_RDONLY|syscall.O_NOATIME|syscall.O_NOCTTY|syscall.O_CLOEXEC, 0644)
		if ek(err) {
			continue
		}

		if *dflag {
			err = syscall.Fdatasync(int(f.Fd()))
		} else {
			err = syscall.Fsync(int(f.Fd()))
		}
		ek(err)
		ek(f.Close())
	}
	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: fsync [-d] FILE ...")
	flag.PrintDefaults()
	os.Exit(2)
}

func ek(err error) bool {
	if err != nil {
		fmt.Fprintln(os.Stderr, "fsync:", err)
		status = 1
		return true
	}
	return false
}
