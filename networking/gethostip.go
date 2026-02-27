// ported from syslinux

package main

import (
	"flag"
	"fmt"
	"net"
	"os"
)

var (
	status = 0
)

func main() {
	flag.Bool("d", false, "print decimal representation")
	flag.Bool("x", false, "print hexadecimal representation")
	flag.Bool("n", false, "print hostname representation")
	flag.Bool("f", false, "print all representation")
	flag.Usage = usage
	flag.Parse()

	output := 0
	flag.Visit(func(f *flag.Flag) {
		switch f.Name {
		case "d":
			output |= 2
		case "x":
			output |= 4
		case "n":
			output |= 1
		case "f":
			output = 7
		}
	})
	if output == 0 {
		output = 7
	}

	if flag.NArg() < 1 {
		usage()
	}

	for _, name := range flag.Args() {
		ips, err := net.LookupIP(name)
		if ek(err) {
			continue
		}

		ip := ips[0].To4()
		if ip == nil {
			fmt.Printf("gethostip: %s: no ipv4 address associated with name\n", name)
			status = 1
			continue
		}

		sep := ""
		if output&1 != 0 {
			fmt.Printf("%s%v", sep, name)
			sep = " "
		}
		if output&2 != 0 {
			fmt.Printf("%s%v", sep, ip)
			sep = " "
		}
		if output&4 != 0 {
			fmt.Printf("%s%02X%02X%02X%02X", sep, ip[0], ip[1], ip[2], ip[3])
		}
		fmt.Println()
	}

	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: gethostip [options] hostname/ip ...")
	flag.PrintDefaults()
	os.Exit(2)
}

func ek(err error) bool {
	if err != nil {
		fmt.Fprintln(os.Stderr, "gethostip: ", err)
		status = 1
		return true
	}
	return false
}
