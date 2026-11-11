// +build linux
// ported from suckless

package main

import (
	"flag"
	"fmt"
	"os"
)

var (
	status int
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	for _, name := range flag.Args() {
		path := fmt.Sprintf("/proc/%s/cwd", name)
		target, err := os.Readlink(path)
		if !ek(err) {
			fmt.Printf("%s: %s\n", name, target)
		}
	}
	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] pid ...")
	flag.PrintDefaults()
	os.Exit(2)
}

func ek(err error) bool {
	if err != nil {
		fmt.Fprintln(os.Stderr, "pwdx:", err)
		status |= 1
		return true
	}
	return false
}
