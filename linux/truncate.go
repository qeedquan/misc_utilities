// ported from suckless
package main

import (
	"flag"
	"fmt"
	"os"
)

var (
	size   = flag.Int64("s", 0, "truncate to size")
	create = flag.Bool("c", false, "create if file doesn't exist")

	status = 0
)

func main() {
	flag.Usage = usage
	flag.Parse()

	sflag := false
	flag.Visit(func(f *flag.Flag) {
		if f.Name == "s" {
			sflag = true
		}
	})
	if flag.NArg() < 1 || !sflag {
		usage()
	}

	for _, name := range flag.Args() {
		flags := os.O_WRONLY
		if *create {
			flags |= os.O_CREATE
		}
		f, err := os.OpenFile(name, flags, 0644)
		if ek(err) {
			continue
		}
		ek(f.Truncate(*size))
		ek(f.Close())
	}
	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: truncate [-cs] file ...")
	flag.PrintDefaults()
	os.Exit(2)
}

func ek(err error) bool {
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		status |= 1
		return true
	}
	return false
}
