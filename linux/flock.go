// +build unix

// ported from toybox
package main

import (
	"flag"
	"fmt"
	"os"
	"strconv"
	"syscall"
)

var (
	sflag = flag.Bool("s", false, "shared lock")
	xflag = flag.Bool("x", false, "exclusive lock")
	uflag = flag.Bool("u", false, "unlock")
	nflag = flag.Bool("n", false, "non-blocking mode")
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() != 1 {
		usage()
	}

	fd, err := strconv.Atoi(flag.Arg(0))
	if err != nil {
		f, err := os.Open(flag.Arg(0))
		ck(err)
		defer f.Close()
		fd = int(f.Fd())
	}

	op := syscall.LOCK_EX
	if *uflag {
		op = syscall.LOCK_UN
	} else if *sflag {
		op = syscall.LOCK_SH
	}

	if *nflag {
		op |= syscall.LOCK_NB
	}

	ck(syscall.Flock(fd, op))
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: flock [-nsux] fd | file")
	flag.PrintDefaults()
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		fmt.Fprintln(os.Stderr, "flock:", err)
		os.Exit(1)
	}
}
