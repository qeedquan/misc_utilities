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
			fmt.Fprintln(os.Stderr, "mtime:", err)
			continue
		}
		fmt.Printf("%11d %v\n", fi.ModTime().Unix(), name)
	}
	os.Exit(status)
}

func usage() {
	fmt.Fprintf(os.Stderr, "usage: mtime file...\n")
	os.Exit(1)
}
