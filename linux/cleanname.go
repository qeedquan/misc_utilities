// ported from plan9 cleanname

package main

import (
	"flag"
	"fmt"
	"os"
	"path/filepath"
)

var (
	dir = flag.String("d", "", "prefix unrooted names with this string")
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	for _, name := range flag.Args() {
		if *dir == "" || filepath.Base(name) != name {
			name = filepath.Clean(name)
		} else {
			name = filepath.Join(*dir, name)
		}
		fmt.Println(name)

	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: cleanname [options] name...")
	flag.PrintDefaults()
	os.Exit(1)
}
