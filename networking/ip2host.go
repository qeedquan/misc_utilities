// ported from http://iq0.com/duffgram/ip2host.c

package main

import (
	"flag"
	"fmt"
	"net"
	"os"
	"strings"
)

func main() {
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}
	for _, ip := range flag.Args() {
		fmt.Println(ip, ip2host(ip))
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] ip ...")
	flag.PrintDefaults()
	os.Exit(2)
}

func ip2host(ip string) string {
	names, err := net.LookupAddr(ip)
	if err != nil {
		return "not found"
	}
	return strings.Join(names, " ")
}
