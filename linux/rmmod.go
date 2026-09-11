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

var (
	fflag = flag.Bool("f", false, "force removal of module (DANGEROUS)")
	wflag = flag.Bool("w", false, "wait for module to be finished before removal")
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	flags := syscall.O_NONBLOCK
	if *fflag {
		flags |= syscall.O_TRUNC
	}
	if *wflag {
		flags &^= syscall.O_NONBLOCK
	}

	status := 0
	for _, name := range flag.Args() {
		if strings.HasSuffix(name, ".ko") {
			name = name[:len(name)-len(".ko")]
		}
		buf := []byte(name)
		buf = append(buf, 0)

		_, _, e := syscall.Syscall(syscall.SYS_DELETE_MODULE, uintptr(unsafe.Pointer(&buf[0])), uintptr(flags), uintptr(0))
		if e != 0 {
			fmt.Fprintln(os.Stderr, "rmmod:", syscall.Errno(e))
			status |= 1
		}
	}
	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: rmmod [-fw] module ...")
	flag.PrintDefaults()
	os.Exit(2)
}
