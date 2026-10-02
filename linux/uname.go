package main

import (
	"bytes"
	"flag"
	"fmt"
	"io"
	"log"
	"os"
	"syscall"
)

var (
	all     = flag.Bool("a", false, "print all information")
	kernel  = flag.Bool("s", false, "print the kernel name")
	node    = flag.Bool("n", false, "print the network node hostname")
	release = flag.Bool("r", false, "print the kernel release")
	version = flag.Bool("v", false, "print the kernel version")
	machine = flag.Bool("m", false, "print the machine hardware name")
	domain  = flag.Bool("d", false, "print the domain name")
)

func main() {
	log.SetFlags(0)
	flag.Usage = usage
	flag.Parse()

	flags := false
	flag.Visit(func(*flag.Flag) {
		flags = true
	})
	if !flags {
		*kernel = true
	}

	var uts syscall.Utsname
	err := syscall.Uname(&uts)
	if err != nil {
		log.Fatal(err)
	}
	buf := new(bytes.Buffer)

	if *all || *kernel {
		xprint(buf, uts.Sysname[:])
	}

	if *all || *node {
		xprint(buf, uts.Nodename[:])
	}

	if *all || *release {
		xprint(buf, uts.Release[:])
	}

	if *all || *version {
		xprint(buf, uts.Version[:])
	}

	if *all || *machine {
		xprint(buf, uts.Machine[:])
	}

	if *all || *domain {
		xprint(buf, uts.Domainname[:])
	}

	str := buf.String()
	fmt.Println(str[:len(str)-1])
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options]")
	flag.PrintDefaults()
	os.Exit(1)
}

func xprint(w io.Writer, s []int8) {
	for _, r := range s {
		if r == '\x00' {
			break
		}
		fmt.Fprintf(w, "%c", r)
	}
	fmt.Fprint(w, " ")
}
