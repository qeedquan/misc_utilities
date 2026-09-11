// +build unix

// ported from toybox
package main

import (
	"flag"
	"fmt"
	"os"
	"os/user"
	"strconv"
	"syscall"
)

var (
	gflag = flag.Bool("g", false, "interpret argument as process group IDs")
	uflag = flag.Bool("u", false, "interpret argument as usernames")
	pflag = flag.Bool("p", true, "interpret argument as process IDs")
	inc   = flag.Int("n", 0, "increment ID")

	status = 0
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	which := syscall.PRIO_PROCESS
	if *gflag {
		which = syscall.PRIO_PGRP
	} else if *uflag {
		which = syscall.PRIO_USER
	}

	for _, arg := range flag.Args() {
		var n int
		var err error

		id := -1
		if *uflag {
			p, err := user.Lookup(arg)
			if err == nil {
				n, err = strconv.Atoi(p.Uid)
			}
		} else {
			n, err = strconv.Atoi(arg)
		}
		if err == nil {
			id = n
		}

		if id < 0 {
			ek(fmt.Errorf("bad %q", arg))
			continue
		}

		prio, err := syscall.Getpriority(which, id)
		if ek(err) {
			continue
		}

		ek(syscall.Setpriority(which, id, prio+*inc))
	}

	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: renice [-gpu] -n increment ID ...")
	flag.PrintDefaults()
	os.Exit(2)
}

func ek(err error) bool {
	if err != nil {
		fmt.Fprintln(os.Stderr, "renice:", err)
		status |= 1
		return true
	}
	return false
}
