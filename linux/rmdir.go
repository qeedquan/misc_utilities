package main

import (
	"flag"
	"fmt"
	"os"
)

var (
	parent = flag.Bool("p", false, "remove parent directories in addition to the directory")
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
		fi, err := os.Stat(name)
		if ck(err) {
			continue
		}

		if !fi.IsDir() {
			fmt.Fprintf(os.Stderr, "rmdir: %s: not a directory\n", name)
			status = 1
			continue
		}

		if *parent {
			err = os.RemoveAll(name)
		} else {
			err = os.Remove(name)
		}
		ck(err)
	}

	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] directory ...")
	flag.PrintDefaults()
	os.Exit(1)
}

func ck(err error) bool {
	if err != nil {
		fmt.Fprintln(os.Stderr, "rmdir:", err)
		status = 1
		return true
	}
	return false
}
