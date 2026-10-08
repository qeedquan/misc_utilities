package main

import (
	"bytes"
	"flag"
	"fmt"
	"log"
	"os"
	"path/filepath"
)

var (
	recursive = flag.Bool("r", false, "recursive")

	status = 0
)

func main() {
	log.SetPrefix("infile: ")
	log.SetFlags(0)
	flag.Usage = usage
	flag.Parse()

	if flag.NArg() < 2 {
		usage()
	}

	args := flag.Args()
	err := infile(args[0], args[1:]...)
	if err != nil {
		log.Fatal(err)
	}
	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: infile [options] file/dir file ...")
	flag.PrintDefaults()
	os.Exit(2)
}

func infile(file string, cmpfiles ...string) error {
	var cmp [][]byte
	for _, name := range cmpfiles {
		b, err := os.ReadFile(name)
		if err != nil {
			fmt.Fprintln(os.Stderr, err)
			status = 1
			cmp = append(cmp, nil)
		} else {
			cmp = append(cmp, b)
		}
	}

	filepath.Walk(file, func(path string, info os.FileInfo, err error) error {
		if info.IsDir() {
			if !*recursive {
				return filepath.SkipDir
			}
			return nil
		}

		buf, err := os.ReadFile(path)
		if err != nil {
			fmt.Fprintln(os.Stderr, err)
			status = 1
			return nil
		}

		in(path, buf, cmpfiles, cmp)
		return nil
	})

	return nil
}

func in(file string, buf []byte, cmpfiles []string, cmp [][]byte) {
	idx := make([]int, len(cmp))
	for {
		eof := true
		for i := 0; i < len(cmp); i++ {
			if idx[i] >= len(buf) {
				continue
			}

			n := bytes.Index(buf[idx[i]:], cmp[i])
			if n < 0 || len(cmp[i]) == 0 {
				l := len(cmp) - 1
				cmp[i], cmp = cmp[l], cmp[:l]
				idx = idx[:l]
			} else {
				fmt.Printf("%s %s offset %d (%#x)\n", file, cmpfiles[i], n, n)
				idx[i] = n + 1
			}
		}

		if eof {
			break
		}
	}
}
