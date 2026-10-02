// ported from suckless
package main

import (
	"flag"
	"fmt"
	"os"
	"syscall"
)

const (
	CONSOLE = "/dev/console"

	VT_LOCKSWITCH   = 0x560b // disallow vt switching
	VT_UNLOCKSWITCH = 0x560c // allow vt switching
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() != 1 {
		usage()
	}

	var vt uintptr
	switch flag.Arg(0) {
	case "y":
		vt = VT_LOCKSWITCH
	case "n":
		vt = VT_UNLOCKSWITCH
	default:
		usage()
	}

	f, err := os.OpenFile(CONSOLE, os.O_WRONLY, 0400)
	ck(err)
	defer f.Close()

	_, _, err = syscall.Syscall(syscall.SYS_IOCTL, f.Fd(), vt, 0)
	xerr, ok := err.(syscall.Errno)
	if ok && xerr == 0 {
		return
	}
	ck(err)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: vtallow n | y")
	flag.PrintDefaults()
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		fmt.Fprintln(os.Stderr, "vtallow:", err)
		os.Exit(1)
	}
}
