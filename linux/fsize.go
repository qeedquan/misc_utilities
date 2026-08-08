// ported from plan9
package main

import (
	"flag"
	"fmt"
	"os"
)

func main() {
	flag.Usage = usage
	flag.Parse()

	if flag.NArg() < 1 {
		usage()
	}

	status := 0
	for _, name := range flag.Args() {
		fi, err := os.Stat(name)
		if err != nil {
			status |= 1
			fmt.Fprintln(os.Stderr, "fsize:", err)
			continue
		}
		fmt.Printf("%s: %d\n", name, fi.Size())
	}
	os.Exit(status)
}

func usage() {
	fmt.Fprintf(os.Stderr, "usage: fsize file...\n")
	os.Exit(1)
}
