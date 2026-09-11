package main

import (
	"flag"
	"fmt"
	"io"
	"net/http"
	"os"
)

var (
	status = 0
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	for _, url := range flag.Args() {
		resp, err := http.Get(url)
		if ek(err) {
			continue
		}
		_, err = io.Copy(os.Stdout, resp.Body)
		ek(err)

		err = resp.Body.Close()
		ek(err)
	}
	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [urls] ...")
	flag.PrintDefaults()
	os.Exit(2)
}

func ek(err error) bool {
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		status |= 1
		return true
	}
	return false
}
