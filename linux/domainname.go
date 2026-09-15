// +build linux

// ported from OpenBSD
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
	var err error
	switch flag.NArg() {
	case 0:
		var uts syscall.Utsname
		err = syscall.Uname(&uts)
		if err == nil {
			name := ""
			for _, r := range uts.Domainname {
				if r == 0 {
					break
				}
				name += string(r)
			}
			fmt.Println(name)
		}
	case 1:
		name := []byte(flag.Arg(0))
		err = syscall.Setdomainname(name)
	default:
		usage()
	}

	if err != nil {
		fmt.Fprintln(os.Stderr, "domainname:", err)
		os.Exit(1)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [name-of-domain]")
	flag.PrintDefaults()
	os.Exit(1)
}
