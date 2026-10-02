// +build linux

// ported from suckless
package main

import (
	"flag"
	"fmt"
	"os"
	"strings"
	"syscall"
	"unsafe"
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}
	args := flag.Args()

	buf, err := os.ReadFile(args[0])
	ck(err)

	opt := []byte(strings.Join(args[1:], " "))
	opt = append(opt, 0)

	_, _, e := syscall.Syscall(syscall.SYS_INIT_MODULE, uintptr(unsafe.Pointer(&buf[0])), uintptr(len(buf)), uintptr(unsafe.Pointer(&opt[0])))
	if e != 0 {
		fmt.Fprintln(os.Stderr, "insmod:", syscall.Errno(e))
		os.Exit(1)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: insmod filename [module options...]")
	flag.PrintDefaults()
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		fmt.Fprintln(os.Stderr, "insmod:", err)
		os.Exit(1)
	}
}
