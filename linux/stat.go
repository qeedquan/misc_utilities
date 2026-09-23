package main

import (
	"flag"
	"fmt"
	"os"
	"path/filepath"
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
			fmt.Fprintln(os.Stderr, err)
			status = 1
			continue
		}

		fmt.Printf("%-7s %v\n", "File:", filepath.Clean(name))
		fmt.Printf("%-7s %v bytes\n", "Size:", fi.Size())
		fmt.Printf("%-7s %v\n", "Access:", fi.Mode())
		fmt.Printf("%-7s %v\n", "Modify:", fi.ModTime())
	}
	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] file ...")
	flag.PrintDefaults()
	os.Exit(1)
}
