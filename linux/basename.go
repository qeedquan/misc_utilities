// ported from plan9 basename
package main

import (
	"flag"
	"fmt"
	"os"
	"path/filepath"
	"strings"
)

func main() {
	flag.Usage = usage
	flag.Parse()

	var name, suffix string
	switch flag.NArg() {
	case 2:
		suffix = flag.Arg(1)
		fallthrough
	case 1:
		name = filepath.Base(flag.Arg(0))
	default:
		usage()
	}
	if suffix != "" && strings.HasSuffix(name, suffix) {
		name = name[:len(name)-len(suffix)]
	}
	fmt.Println(name)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: name [suffix]")
	flag.PrintDefaults()
	os.Exit(1)
}
