package main

import (
	"flag"
	"fmt"
	"io/fs"
	"log"
	"os"
	"path/filepath"
)

var (
	recursive = flag.Bool("r", false, "recursive")
	topdir    = "."
	chosen    []byte
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("oneify: ")

	flag.Usage = usage
	flag.Parse()

	var err error
	switch flag.NArg() {
	case 2:
		topdir = flag.Arg(1)
		fallthrough

	case 1:
		chosen, err = os.ReadFile(flag.Arg(0))
		check(err)

	default:
		usage()
	}

	if !filepath.IsAbs(topdir) {
		cwd, err := os.Getwd()
		check(err)
		topdir = filepath.Join(cwd, topdir)
	}
	topdir = filepath.Clean(topdir)

	filepath.Walk(topdir, walk)
}

func walk(path string, info fs.FileInfo, err error) error {
	if err != nil {
		return err
	}

	if !*recursive {
		dir := filepath.Dir(path)
		if len(dir) > len(topdir) {
			return filepath.SkipDir
		}
	}

	if !info.IsDir() {
		fmt.Println("Replacing", path)
		err = os.WriteFile(path, chosen, 0644)
		if err != nil {
			fmt.Println(err)
		}
	}

	return nil
}

func usage() {
	fmt.Println("usage: [options] file [path]")
	flag.PrintDefaults()
	os.Exit(2)
}

func check(err error) {
	if err != nil {
		log.Fatal(err)
	}
}
