package main

import (
	"bufio"
	"flag"
	"fmt"
	"io"
	"io/fs"
	"log"
	"os"
	"path/filepath"
)

var (
	start     = flag.Int64("start", 0, "start offset")
	maxsize   = flag.Int64("maxsize", 0, "max size")
	blocksize = flag.Int64("blocksize", 0, "block size")
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("gen-raw-disk-image: ")

	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 2 {
		usage()
	}

	f, err := os.Create(flag.Arg(0))
	check(err)

	pos := int64(0)
	w := bufio.NewWriter(f)
	if *start > 0 {
		w.Write(make([]byte, *start))
		pos += *start
	}

	for i := 1; i < flag.NArg(); i++ {
		filepath.Walk(flag.Arg(i), func(path string, info fs.FileInfo, err error) error {
			return walk(w, &pos, path, info, err)
		})
	}

	check(w.Flush())
	if *maxsize > 0 {
		check(f.Truncate(*maxsize))
	}
	check(f.Close())
}

func check(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] disk.img file ...")
	flag.PrintDefaults()
	os.Exit(2)
}

func walk(w *bufio.Writer, pos *int64, path string, info fs.FileInfo, err error) error {
	if err != nil {
		log.Fatalf("%v: %v", path, err)
	}

	if info.IsDir() {
		return nil
	}

	f, err := os.Open(path)
	check(err)

	n, err := io.Copy(w, f)
	check(err)
	f.Close()

	if *blocksize > 0 {
		pad := *blocksize - (n % *blocksize)
		w.Write(make([]byte, pad))
		n += pad

		fmt.Printf("%#08x %#08x (%d blocks) %s\n", *pos, *pos+n-1, n / *blocksize, path)
	} else {
		fmt.Printf("%#08x %#08x %s\n", *pos, *pos+n-1, path)
	}

	*pos += n
	return nil
}
