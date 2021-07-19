package main

import (
	"flag"
	"fmt"
	"log"
	"net"
	"os"
)

var (
	network = flag.String("n", "udp", "use network transport")
	pktsize = flag.Int("s", 8192, "packet size")
)

func main() {
	flag.Usage = usage
	flag.Parse()

	address := ":0"
	if flag.NArg() > 0 {
		address = flag.Arg(0)
	}

	ln, err := net.ListenPacket(*network, address)
	if err != nil {
		log.Fatal(err)
	}
	defer ln.Close()

	fmt.Printf("listening on %v\n", ln.LocalAddr())
	buf := make([]byte, *pktsize)
	for {
		n, addr, err := ln.ReadFrom(buf)
		if err != nil {
			fmt.Fprintln(os.Stderr, "lstpkt: ", err)
			break
		}

		if n > 16 {
			n = 16
		}
		fmt.Printf("%v: [%q]\n", addr, buf[:n])
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: lstpkt [options] [addr]")
	flag.PrintDefaults()
	os.Exit(2)
}
