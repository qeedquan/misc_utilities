package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"path/filepath"
	"sort"
	"unicode/utf8"
)

var (
	descending = flag.Bool("d", false, "sort in descending order")
	recursive  = flag.Bool("r", false, "recursive")

	status = 0
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("namelen: ")
	flag.Usage = usage
	flag.Parse()

	args := flag.Args()
	if len(args) < 1 {
		args = append(args, ".")
	}

	var fis []os.FileInfo
	for _, path := range args {
		fi, err := ln(path, *recursive)
		if err != nil {
			fmt.Fprintln(os.Stderr, "namelen:", err)
			status = 1
			continue
		}
		fis = append(fis, fi...)
	}

	sort.Slice(fis, func(i, j int) bool {
		x := utf8.RuneCountInString(fis[i].Name())
		y := utf8.RuneCountInString(fis[j].Name())
		if *descending {
			return x > y
		}
		return x < y
	})
	for _, fi := range fis {
		fmt.Printf("%v %v", utf8.RuneCountInString(fi.Name()), fi.Name())
		if fi.IsDir() {
			fmt.Printf("/")
		}
		fmt.Println()
	}

	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] [file/dir] ...")
	flag.PrintDefaults()
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func ln(path string, recursive bool) ([]os.FileInfo, error) {
	f, err := os.Open(path)
	if err != nil {
		return nil, err
	}
	defer f.Close()

	fi, err := f.Stat()
	if err != nil {
		return nil, err
	}

	var fis []os.FileInfo
	switch {
	case !fi.IsDir():
		fis = append(fis, fi)
	case !recursive:
		fis, err = f.Readdir(-1)
	default:
		filepath.Walk(path, func(path string, info os.FileInfo, err error) error {
			if err != nil {
				fmt.Fprintln(os.Stderr, err)
			}
			fis = append(fis, info)
			return nil
		})
	}

	return fis, nil
}
