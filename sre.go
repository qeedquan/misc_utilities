package main

import (
	"flag"
	"fmt"
	"io/fs"
	"os"
	"path/filepath"
	"regexp"
)

var (
	write  bool
	escape bool

	rootdir string
	refl    *regexp.Regexp
	refd    *regexp.Regexp
	repl    []byte
)

func main() {
	parseflags()
	filepath.Walk(rootdir, walk)
}

func parseflags() {
	flag.BoolVar(&escape, "e", false, "escape regex")
	flag.BoolVar(&write, "w", false, "write to file")
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 4 {
		usage()
	}

	rootdir = flag.Arg(0)
	pattern := flag.Arg(2)
	if escape {
		pattern = regexp.QuoteMeta(pattern)
	}
	refl = regexp.MustCompile(flag.Arg(1))
	refd = regexp.MustCompile(pattern)
	repl = []byte(flag.Arg(3))
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] <path> <filepattern> <pattern> <replacement>")
	flag.PrintDefaults()
	os.Exit(2)
}

func walk(path string, info fs.FileInfo, err error) error {
	if !refl.MatchString(path) {
		return nil
	}

	data, err := os.ReadFile(path)
	if err != nil {
		fmt.Println("ERROR:", err)
		return nil
	}

	matches := refd.FindAll(data, -1)
	if len(matches) == 0 {
		return nil
	}

	fmt.Printf("%v (%d match)\n", path, len(matches))
	for i := range matches {
		fmt.Printf("%q\n", matches[i])
	}
	fmt.Println()

	data = refd.ReplaceAll(data, repl)
	if write {
		err = os.WriteFile(path, data, 0644)
		if err != nil {
			fmt.Println("ERROR:", err)
		}
	}

	return nil
}
