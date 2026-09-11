package main

import (
	"flag"
	"fmt"
	"log"
	"net"
	"os"
)

func main() {
	log.SetFlags(0)
	flag.Parse()

	nis, err := net.Interfaces()
	ck(err)
	for _, ni := range nis {
		addrs, err := ni.Addrs()
		if ek(err) {
			continue
		}

		fmt.Printf("%d %s\n", ni.Index, ni.Name)
		fmt.Printf("hwaddr: %v\n", ni.HardwareAddr)
		fmt.Printf("mtu: %v\n", ni.MTU)
		fmt.Printf("flags: %v\n", ni.Flags)
		for _, addr := range addrs {
			fmt.Println("addr:", addr)
		}
		fmt.Println()
	}
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func ek(err error) bool {
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		return true
	}
	return false
}
