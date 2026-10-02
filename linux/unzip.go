package main

import (
	"archive/zip"
	"flag"
	"fmt"
	"io"
	"log"
	"os"
	"path/filepath"
)

var (
	exdir = flag.String("d", "", "extract to external dir")

	status = 0
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("unzip: ")
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	z, err := zip.OpenReader(flag.Arg(0))
	defer z.Close()
	ck(err)

	for _, f := range z.File {
		r, err := f.Open()
		if ek(err) {
			continue
		}

		name := filepath.Join(*exdir, f.Name)
		dir := filepath.Dir(name)
		os.MkdirAll(dir, 0755)

		fmt.Println(name)
		w, err := os.Create(name)
		if ek(err) {
			continue
		}

		_, err = io.Copy(w, r)
		ek(err)
		r.Close()
		ek(w.Close())
	}

	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] file")
	flag.PrintDefaults()
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func ek(err error) bool {
	if err != nil {
		fmt.Fprintln(os.Stderr, "unzip:", err)
		status = 1
		return true
	}
	return false
}
