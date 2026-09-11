package main

import (
	"bufio"
	"flag"
	"fmt"
	"io"
	"os"
	"time"
)

var (
	status = 0

	format   = flag.String("f", "15:04:05.000000", "time format string")
	duration = flag.Duration("d", 0, "duration between sampling")
)

func main() {
	flag.Usage = usage
	flag.Parse()

	if flag.NArg() < 1 {
		ts("<stdin>", os.Stdin)
	} else {
		for _, name := range flag.Args() {
			fd, err := os.Open(name)
			if ek(err) {
				continue
			}
			ts(name, fd)
			fd.Close()
		}
	}

	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: ts [options] file ...")
	flag.PrintDefaults()
	os.Exit(2)
}

func ek(err error) bool {
	if err != nil {
		fmt.Fprintln(os.Stderr, "ts:", err)
		status = 1
		return true
	}
	return false
}

func ts(name string, r io.Reader) {
	s := bufio.NewScanner(r)

	var m time.Time
	for s.Scan() {
		n := time.Now()
		if n.Sub(m) < *duration {
			continue
		}
		m = n

		l := s.Text()
		t := n.Format(*format)
		if flag.NArg() < 1 {
			fmt.Printf("[%v] %s\n", t, l)
		} else {
			fmt.Printf("%s: [%v] %s\n", name, t, l)
		}
	}
}
