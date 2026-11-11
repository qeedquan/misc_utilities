package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"strconv"
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("fillfile: ")
	flag.Usage = usage
	flag.Parse()

	var args []string
	switch flag.NArg() {
	case 4:
		args = flag.Args()
	case 3:
		args = append(flag.Args(), "0")
		args[1], args[3] = args[3], args[1]
	default:
		usage()
	}

	offset, _ := strconv.ParseInt(args[1], 0, 64)
	value, _ := strconv.ParseInt(args[2], 0, 64)
	length, _ := strconv.ParseInt(args[3], 0, 64)

	buf, err := os.ReadFile(flag.Arg(0))
	ck(err)
	for i := offset; i < offset+length; i++ {
		if i < 0 || i >= int64(len(buf)) {
			break
		}
		buf[i] = byte(value)
	}
	ck(os.WriteFile(flag.Arg(0), buf, 644))
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: fillfile file [offset] value length")
	flag.PrintDefaults()
	os.Exit(2)
}
