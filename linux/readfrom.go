package main

import (
	"flag"
	"fmt"
	"io"
	"log"
	"os"
)

var (
	mode = flag.Int("m", 0, "mode")
)

func main() {
	log.SetPrefix("readfrom: ")
	log.SetFlags(0)
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() != 3 {
		usage()
	}

	start, end := location(flag.Arg(0))
	fmt.Printf("reading %s at %#x-%#x\n", flag.Arg(1), start, end)

	r, err := os.Open(flag.Arg(1))
	ck(err)
	defer r.Close()

	_, err = r.Seek(int64(start), io.SeekStart)
	ck(err)

	b := make([]byte, end-start+1)
	_, err = io.ReadAtLeast(r, b, len(b))
	ck(err)

	w, err := os.Create(flag.Arg(2))
	ck(err)

	_, err = w.Write(b)
	ck(err)
	ck(w.Close())
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: readfrom [options] location input output")
	flag.PrintDefaults()
	os.Exit(2)
}

func location(loc string) (start, end uint64) {
	var (
		n     int
		off   uint64
		count uint64
		err   error
	)
	switch *mode {
	case 0:
		n, err = fmt.Sscanf(loc, "%v-%v", &start, &end)
	case 1:
		n, err = fmt.Sscanf(loc, "%v-%v", &off, &count)
		start, end = off, off+count
	default:
		log.Fatal("unknown mode %d", *mode)
	}

	if n != 2 || err != nil {
		log.Fatal("invalid location format %q", loc)
	}
	if end < start {
		start, end = end, start
	}
	return
}
