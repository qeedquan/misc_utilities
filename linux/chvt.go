// +build linux

// ported from toybox
package main

import (
	"flag"
	"fmt"
	"os"
	"strconv"
	"syscall"
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	vtnum, err := strconv.Atoi(flag.Arg(0))
	ck(err)

	consoles := []string{"/dev/console", "/dev/vc/0", "/dev/tty"}
	var f *os.File
	for _, cc := range consoles {
		f, err = os.OpenFile(cc, os.O_RDWR, 0644)
		if err == nil {
			break
		}
	}
	if f == nil {
		fmt.Fprintln(os.Stderr, "chvt: couldn't get file descriptor referring to console")
		os.Exit(1)
	}

	fd := f.Fd()
	_, _, err = syscall.Syscall(syscall.SYS_IOCTL, fd, 0x5606, uintptr(vtnum))
	if err != nil {
		_, _, err = syscall.Syscall(syscall.SYS_IOCTL, fd, 0x5607, uintptr(vtnum))
	}
	f.Close()

	if err != nil {
		fmt.Fprintln(os.Stderr, "chvt:", err)
		os.Exit(1)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: chvt N")
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		fmt.Fprintln(os.Stderr, "chvt:", err)
		os.Exit(1)
	}
}
