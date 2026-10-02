package main

import (
	"flag"
	"fmt"
	"io"
	"os"
	"strconv"
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() == 0 {
		buf, _ := io.ReadAll(os.Stdin)
		fmt.Println(strconv.Quote(string(buf)))
	} else {
		for _, arg := range flag.Args() {
			fmt.Println(strconv.Quote(arg))
		}
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: string ...")
	flag.PrintDefaults()
	os.Exit(2)
}
