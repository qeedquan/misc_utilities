// +build linux

// ported from toybox

package main

import (
	"flag"
	"fmt"
	"os"
	"syscall"
)

const (
	BLKRRPART = 0x125f
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
		f, err := os.Open(name)
		if ek(err) {
			continue
		}
		_, _, e := syscall.Syscall(syscall.SYS_IOCTL, f.Fd(), BLKRRPART, 0)
		if e != 0 {
			ek(syscall.Errno(e))
		}
		ek(f.Close())
	}
	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: partprobe DEVICE ...")
	flag.PrintDefaults()
	os.Exit(2)
}

func ek(err error) bool {
	if err != nil {
		fmt.Fprintln(os.Stderr, "partprobe:", err)
		status |= 1
		return true
	}
	return false
}
