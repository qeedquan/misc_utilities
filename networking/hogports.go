// hogs a range of ports, ported from 9front
package main

import (
	"flag"
	"fmt"
	"net"
	"os"
	"strings"
	"sync"
)

var wg sync.WaitGroup

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() == 0 {
		usage()
	}

	for i := 0; i < flag.NArg(); i++ {
		hogRange(flag.Arg(i))
	}

	wg.Wait()
	fmt.Fprintln(os.Stderr, "failed to hog any ports")
	os.Exit(1)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: hogports net:[host]:portrange...")
	flag.PrintDefaults()
	os.Exit(2)
}

func hogRange(str string) {
	var network, hostname string
	var start, end int

	fields := strings.Split(str, ":")
	switch len(fields) {
	case 3:
		hostname = fields[1]
		fields[1] = fields[2]
		fallthrough
	case 2:
		network = fields[0]
		n, err := fmt.Sscanf(fields[1], "%d-%d", &start, &end)
		if n == 1 {
			end = start
			break
		}
		if !(n != 2 || err != nil) {
			break
		}
		fallthrough
	default:
		fmt.Fprintf(os.Stderr, "bad syntax: %q\n", str)
		return
	}

	if end < start {
		start, end = end, start
	}

	for port := start; port <= end; port++ {
		wg.Add(1)
		go hogPort(network, fmt.Sprintf("%s:%d", hostname, port))
	}
}

func hogPort(network, laddr string) {
	defer wg.Done()

	ln, err := net.Listen(network, laddr)
	if err != nil {
		fmt.Fprintf(os.Stderr, "failed to hog %q: %q\n", network+laddr, err)
		return
	}
	defer ln.Close()

	for {
		conn, err := ln.Accept()
		if err != nil {
			continue
		}
		conn.Close()
	}
}
