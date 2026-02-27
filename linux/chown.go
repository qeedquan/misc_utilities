package main

import (
	"flag"
	"fmt"
	"os"
	"strconv"
)

func main() {
	flag.Usage = usage
	flag.Parse()

	args := flag.Args()
	if len(args) < 3 {
		usage()
	}

	status := 0
	uid := atoi(args[0])
	gid := atoi(args[1])

	for i := 2; i < len(args); i++ {
		err := os.Chown(args[i], uid, gid)
		if err != nil {
			fmt.Fprintln(os.Stderr, err)
			status = 1
		}
	}

	os.Exit(status)
}

func atoi(s string) int {
	n, err := strconv.Atoi(s)
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}
	return n
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: chown [options] uid gid file...")
	flag.PrintDefaults()
	os.Exit(1)
}
