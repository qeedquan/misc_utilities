package main

import (
	"flag"
	"fmt"
	"os"
	"path/filepath"
	"regexp"
)

var (
	aflag = flag.Bool("a", false, "append string to file instead of prepending")

	status = 0
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 2 {
		usage()
	}

	re, err := regexp.Compile(flag.Arg(0))
	ck(err)

	dirs := flag.Args()[2:]
	if len(dirs) == 0 {
		dirs = append(dirs, ".")
	}

	var paths []string
	for _, dir := range dirs {
		filepath.Walk(dir, func(path string, info os.FileInfo, err error) error {
			if ek(err) {
				return nil
			}
			if re.MatchString(path) {
				paths = append(paths, path)
			}
			return nil
		})
	}

	for _, path := range paths {
		dir, base := filepath.Split(path)
		newPath := filepath.Join(dir, flag.Arg(1)+base)
		if *aflag {
			newPath = filepath.Join(dir, base+flag.Arg(1))
		}
		ek(os.Rename(path, newPath))
	}

	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] filespec string [dir] ...")
	flag.PrintDefaults()
	os.Exit(1)
}

func ek(err error) bool {
	if err != nil {
		fmt.Fprintln(os.Stderr, "prepend:", err)
		status |= 1
		return true
	}
	return false
}

func ck(err error) {
	if ek(err) {
		os.Exit(status)
	}
}
