package main

import (
	"encoding/binary"
	"flag"
	"fmt"
	"net"
	"net/netip"
	"os"
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	for i := 0; i < flag.NArg(); i++ {
		ip := flag.Arg(i)
		addr := net.ParseIP(ip)
		addr4 := addr.To4()
		addr6 := addr.To16()

		fmt.Printf("%v:\n", ip)
		if len(addr4) != 0 {
			netaddr4 := netip.AddrFrom4([4]byte(addr4))
			fmt.Printf("  ipv4: %v | %#x\n",
				netaddr4,
				binary.BigEndian.Uint32(addr4),
			)
		}

		if len(addr6) != 0 {
			netaddr6 := netip.AddrFrom16([16]byte(addr6))
			fmt.Printf("  ipv6: %v | 0x%x%x%x%x\n",
				netaddr6,
				binary.BigEndian.Uint32(addr6),
				binary.BigEndian.Uint32(addr6[4:]),
				binary.BigEndian.Uint32(addr6[8:]),
				binary.BigEndian.Uint32(addr6[12:]),
			)
		}
		fmt.Println()
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: <ip> ...")
	flag.PrintDefaults()
	os.Exit(2)
}
