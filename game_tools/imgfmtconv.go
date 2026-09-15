package main

import (
	"flag"
	"fmt"
	"log"
	"os"

	"github.com/qeedquan/go-media/image/imageutil"
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("imgfmtconv: ")

	flag.Parse()
	if flag.NArg() != 2 {
		usage()
	}

	m, err := imageutil.LoadRGBAFile(flag.Arg(0))
	ck(err)

	err = imageutil.WriteRGBAFile(flag.Arg(1), m)
	ck(err)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] in out")
	flag.PrintDefaults()
	os.Exit(2)
}
