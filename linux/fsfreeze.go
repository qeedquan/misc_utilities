// +build linux

// ported from toybox

package main

import (
	"flag"
	"fmt"
	"os"
	"syscall"
	"unsafe"
)

const (
	FIFREEZE = 0xc0045877
	FITHAW   = 0xc0045878
)

var (
	fflag = flag.Bool("f", false, "freeze a filesystem")
	uflag = flag.Bool("u", false, "unfreeze a filesystem")
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() != 1 {
		usage()
	}

	f, err := os.Open(flag.Arg(0))
	ck(err)

	flags := FITHAW
	if *fflag {
		flags = FIFREEZE
	}
	p := int64(1)
	_, _, e := syscall.Syscall(syscall.SYS_IOCTL, f.Fd(), uintptr(flags), uintptr(unsafe.Pointer(&p)))
	if e != 0 {
		ck(syscall.Errno(e))
	}
	ck(f.Close())
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: fsfreeze [-f] mountpoint")
	flag.PrintDefaults()
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		fmt.Fprintln(os.Stderr, "fsfreeze:", err)
		os.Exit(1)
	}
}
