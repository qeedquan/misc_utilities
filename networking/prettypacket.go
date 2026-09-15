// ported from hexinject
// http://hexinject.sourceforge.net/

package main

import (
	"bytes"
	"flag"
	"fmt"
	"log"
	"os"
)

const (
	COLS      = 80
	BYTE_MULT = 3
)

var (
	example = flag.String("x", "", "print example output for packet type")
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("prettypacket: ")

	flag.Usage = usage
	flag.Parse()
	if *example != "" {
		decodeLayer2(packet(*example), 0)
		fmt.Println()
	} else {
		loop()
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options]")
	flag.PrintDefaults()
	fmt.Fprintln(os.Stderr, "\navailable examples: tcp udp icmp igmp arp")
	os.Exit(2)
}

func packet(str string) []byte {
	switch str {
	case "tcp":
		return []byte(tcp)
	case "udp":
		return []byte(udp)
	case "icmp":
		return []byte(icmp)
	case "igmp":
		return []byte(igmp)
	case "arp":
		return []byte(arp)
	}

	log.Fatalf("unknown packet type %q", str)
	return nil
}

func printISL(buf []byte) {
	fmt.Println("\nISL Header:")
	if len(buf) < 30 {
		fmt.Println(" invalid header size")
		return
	}

	buf = printField(buf, 5, "Destination")

	prot := int(buf[0]) >> 4

	buf = printField(buf, 1, "Type/User")
	buf = printField(buf, 6, "Source")
	buf = printField(buf, 2, "Length")
	buf = printField(buf, 1, "DSAP")
	buf = printField(buf, 1, "SSAP")
	buf = printField(buf, 1, "Control")
	buf = printField(buf, 3, "HSA")
	buf = printField(buf, 2, "Vlan ID/BPDU")
	buf = printField(buf, 2, "Index")
	buf = printField(buf, 2, "RES")

	if prot == 0 {
		printEthernet(buf[:len(buf)-4])
	} else {
		printPayload(buf[:len(buf)-4])
	}

	fmt.Println("\nISL Header (end):")
	printField(buf, 4, "Frame check seq.")
}

func printLLC(buf []byte) {
	fmt.Println("\nLogical-Link Control Header:")
	if len(buf) < 3 {
		fmt.Println(" invalid header size")
		return
	}

	dsap := buf[0]
	buf = printField(buf, 1, "DSAP")

	ssap := buf[0]
	buf = printField(buf, 1, "SSAP")

	buf = printField(buf, 1, "Control field")

	if dsap == 0x42 && ssap == 0x42 {
		printSTP(buf)
	} else if dsap == 0xaa && ssap == 0xaa {
		if len(buf) < 8 {
			fmt.Println("invalid header size")
			return
		}

		buf = printField(buf, 3, "Organization code")

		pid := extractBE(buf, 2)
		buf = printField(buf, 2, "PID")

		if pid == 0x2004 {
			printDTP(buf)
		} else {
			printPayload(buf)
		}
	}
}

func printDTP(buf []byte) {
	fmt.Println("\nDynamic Trunking Protocol Header:")
	if len(buf) < 29 {
		fmt.Println(" invalid header size")
		return
	}

	buf = printField(buf, 1, "Version")
	buf = printField(buf, 8, "Domain")

	buf = printField(buf, 5, "Status")
	buf = printField(buf, 5, "DTP Type")

	buf = printField(buf, 8, "Neighbor")
	buf = printField(buf, 2, "")

	printPayload(buf)
}

func printSTP(buf []byte) {
	fmt.Println("\nSpanning Tree Protocol Header:")
	if len(buf) < 38 {
		fmt.Println(" invalid header size")
		return
	}

	buf = printField(buf, 2, "Protocol Identifier")
	buf = printField(buf, 1, "Protocol Version Identifier")

	buf = printField(buf, 1, "BPDU Type")
	buf = printField(buf, 1, "BPDU Flags")

	buf = printField(buf, 2, "Root Priority/System ID Extension")
	buf = printField(buf, 6, "Root System ID")

	buf = printField(buf, 4, "Root Path Cost")

	buf = printField(buf, 2, "Bridge Priority/System ID Extension")
	buf = printField(buf, 6, "Bridge System ID")

	buf = printField(buf, 2, "Protocol Identifier")
	buf = printField(buf, 2, "Protocol Identifier")
	buf = printField(buf, 2, "Protocol Identifier")
}

func printEthernet(buf []byte) {
	fmt.Println("\nEthernet Header: ")
	if len(buf) < 14 {
		fmt.Println(" invalid header size")
		return
	}

	dstmac := extractLE(buf, 6)
	buf = printField(buf, 6, "Destination hardware address")
	buf = printField(buf, 6, "Source hardware address")

	prot := extractBE(buf, 2)
	buf = printField(buf, 2, "Length/Type")

	if prot > 1500 {
		decodeLayer3(buf, prot)
	} else {
		decodeLayer2(buf, dstmac)
	}
}

func printIP(buf []byte) {
	fmt.Println("\nIP Header:")
	if len(buf) < 20 {
		fmt.Println(" invalid header size")
		return
	}

	buf = printField(buf, 1, "Version / Header length")
	buf = printField(buf, 1, "Tos / DFS")
	buf = printField(buf, 2, "Total length")
	buf = printField(buf, 2, "ID")
	buf = printField(buf, 2, "Flags / Fragment offset")
	buf = printField(buf, 1, "TTL")

	prot := extractLE(buf, 1)
	buf = printField(buf, 1, "Protocol")

	buf = printField(buf, 2, "Checksum")
	buf = printField(buf, 4, "Source address")
	buf = printField(buf, 4, "Destination address")

	decodeLayer4(buf, prot)
}

func printARP(buf []byte) {
	fmt.Println("\nARP Header:")
	if len(buf) < 28 {
		fmt.Println(" invalid header size")
		return
	}

	buf = printField(buf, 2, "Hardware type")
	buf = printField(buf, 2, "Protocol type")

	hs := int(buf[0])
	buf = printField(buf, 1, "Hardware size")

	ps := int(buf[0])
	buf = printField(buf, 1, "Protocol size")

	buf = printField(buf, 2, "Opcode")

	buf = printField(buf, hs, "Sender hardware address")
	buf = printField(buf, ps, "Sender protocol address")
	buf = printField(buf, hs, "Target hardware address")
	buf = printField(buf, ps, "Target protocol address")

	printPayload(buf)
}

func printPayload(buf []byte) {
	if len(buf) < 1 {
		return
	}
	fmt.Println("\nPayload or Trailer:")

	bpp := COLS / BYTE_MULT
	for j := 0; j < len(buf); {
		for i := 0; i < bpp && j < len(buf); i, j = i+1, j+1 {
			fmt.Printf(" %02X", buf[j])
		}
		fmt.Println()
	}
}

func printICMP(buf []byte) {
	fmt.Println("\nICMP Header:")
	if len(buf) < 8 {
		fmt.Println(" invalid header size")
		return
	}

	buf = printField(buf, 1, "Type")
	buf = printField(buf, 1, "Code")
	buf = printField(buf, 2, "Checksum")
	buf = printField(buf, 2, "ID")
	buf = printField(buf, 2, "Sequence Number")

	printPayload(buf)
}

func printIGMP(buf []byte) {
	fmt.Println("\nIGMP Header:")
	if len(buf) < 8 {
		fmt.Println(" invalid header size")
		return
	}

	buf = printField(buf, 1, "Type")
	buf = printField(buf, 1, "Max response time")
	buf = printField(buf, 2, "Checksum")
	buf = printField(buf, 4, "Group address")

	printPayload(buf)
}

func printTCP(buf []byte) {
	fmt.Println("\nTCP header:")
	if len(buf) < 8 {
		fmt.Println(" invalid header size")
		return
	}

	buf = printField(buf, 2, "Source port")
	buf = printField(buf, 2, "Destination port")
	buf = printField(buf, 4, "Sequence number")
	buf = printField(buf, 4, "Acknowledgement number")
	buf = printField(buf, 1, "Header length")
	buf = printField(buf, 1, "Flags")
	buf = printField(buf, 2, "Window")
	buf = printField(buf, 2, "Checksum")
	buf = printField(buf, 2, "Urgent Pointer")

	printPayload(buf)
}

func printUDP(buf []byte) {
	fmt.Println("\nUDP header:")
	if len(buf) < 8 {
		fmt.Println(" invalid header size")
		return
	}

	buf = printField(buf, 2, "Source port")
	buf = printField(buf, 2, "Destination port")
	buf = printField(buf, 2, "Length")
	buf = printField(buf, 2, "Checksum")

	printPayload(buf)
}

func extractLE(buf []byte, size int) uint64 {
	v := uint64(0)
	for i := 0; i < size; i++ {
		v |= uint64(buf[i]) << uint(8*i)
	}
	return v
}

func extractBE(buf []byte, size int) uint64 {
	v := uint64(0)
	for i := 0; i < size; i++ {
		v |= uint64(buf[i]) << uint(8*(size-i-1))
	}
	return v
}

func printField(buf []byte, size int, text string) []byte {
	hex := fmt.Sprintf("% -X", buf[:size])
	fmt.Printf(" %-24s %s\n", hex, text)
	return buf[size:]
}

func decodeLayer2(buf []byte, prot uint64) {
	const llc1 = 0x0180C20000
	const llc2 = 0x01000CCCCCCC

	if len(buf) < 1 {
		return
	}

	switch {
	case bytes.Compare(buf, []byte("\x01\x00\x0C\x00\x00")) == 0,
		bytes.Compare(buf, []byte("\x03\x00\x0c\x00\x00")) == 0:
		printISL(buf)

	case prot/256 == llc1, prot == llc2:
		printLLC(buf)

	default:
		printEthernet(buf)
	}
}

func decodeLayer3(buf []byte, prot uint64) {
	if len(buf) < 1 {
		return
	}

	// if last field value (of ethernet header) is less than or equal to 1500
	// then it is a length, otherwise, it is a protocol (IEEE 802.3)
	if prot <= 0xffff {
		switch prot {
		case 0x0800:
			printIP(buf)
		case 0x0806:
			printARP(buf)
		default:
			printPayload(buf)
		}
	} else {
		printPayload(buf)
	}
}

func decodeLayer4(buf []byte, prot uint64) {
	if len(buf) < 1 {
		return
	}

	switch prot {
	case 1:
		printICMP(buf)
	case 2:
		printIGMP(buf)
	case 6:
		printTCP(buf)
	case 17:
		printUDP(buf)
	default:
		printPayload(buf)
	}
}

func loop() {
	var buf [8192]byte
	for {
		n, err := os.Stdin.Read(buf[:])
		if err != nil {
			break
		}
		decodeLayer2(buf[:n], 0)
		fmt.Printf("\n ----------- \n")
	}
	fmt.Println()
}

const (
	arp = "\xFF\xFF\xFF\xFF\xFF\xFF\xAA\x00\x04\x00\x0A\x04\x08\x06\x00\x01\x08\x00\x06\x04\x00\x01\xAA\x00\x04\x00\x0A\x04\xC0\xA8\x01\x09\x00\x00\x00\x00\x00\x00\xC0\xA8\x01\x04"

	tcp = "\x1C\xAF\xF7\x6B\x0E\x4D\xAA\x00\x04\x00\x0A\x04\x08\x00\x45\x00\x00\x34\x5A\xAE\x40\x00\x40\x06\x5E\x67\xC0\xA8\x01\x09\x58\xBF\x67\x3E\x9B\x44\x00\x50\x8E\xB5\xC6\xAC\x15\x93\x47\x9E\x80\x10\x00\x58\xA5\xA0\x00\x00\x01\x01\x08\x0A\x00\x09\xC3\xB2\x42\x5B\xFA\xD6"

	icmp = "\x1C\xAF\xF7\x6B\x0E\x4D\xAA\x00\x04\x00\x0A\x04\x08\x00\x45\x00\x00\x54\x00\x00\x40\x00\x40\x01\x54\x4E\xC0\xA8\x01\x09\xC0\xA8\x64\x01\x08\x00\x34\x98\xD7\x10\x00\x01\x5B\x68\x98\x4C\x00\x00\x00\x00\x2D\xCE\x0C\x00\x00\x00\x00\x00\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2A\x2B\x2C\x2D\x2E\x2F\x30\x31\x32\x33\x34\x35\x36\x37"

	udp = "\x1C\xAF\xF7\x6B\x0E\x4D\xAA\x00\x04\x00\x0A\x04\x08\x00\x45\x00\x00\x3C\x9B\x23\x00\x00\x40\x11\x70\xBC\xC0\xA8\x01\x09\xD0\x43\xDC\xDC\x91\x02\x00\x35\x00\x28\x6F\x0B\xAE\x9C\x01\x00\x00\x01\x00\x00\x00\x00\x00\x00\x03\x77\x77\x77\x06\x67\x6F\x6F\x67\x6C\x65\x03\x63\x6F\x6D\x00\x00\x01\x00\x01"

	igmp = "\x1C\xAF\xF7\x6B\x0E\x4D\xAA\x00\x04\x00\x0A\x04\x08\x00\x45\x00\x00\x1C\x00\x00\x40\x00\x40\x02\x54\x4E\xC0\xA8\x01\x09\xC0\xA8\x64\x01\x11\xFF\x0D\xFF\xE0\x00\x00\x01"
)
