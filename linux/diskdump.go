package main

import (
	"bytes"
	"flag"
	"fmt"
	"io"
	"os"
)

var (
	status = 0
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}
	for _, name := range flag.Args() {
		ek(dump(name))
	}
	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: diskdump file ...")
	flag.PrintDefaults()
	os.Exit(2)
}

func ek(err error) {
	if err != nil {
		fmt.Fprintln(os.Stderr, "diskdump:", err)
		status = 1
	}
}

func dump(name string) error {
	f, err := os.Open(name)
	if err != nil {
		return err
	}
	defer f.Close()

	var mbr [512]byte
	_, err = io.ReadAtLeast(f, mbr[:], len(mbr))
	if err != nil {
		return fmt.Errorf("failed to read mbr: %v", err)
	}

	if mbr[0x1fe] != 0x55 || mbr[0x1ff] != 0xaa {
		return fmt.Errorf("invalid mbr boot record signature")
	}

	var part [4]Part
	for i := range part {
		part[i] = readPart(mbr[0x1be+i*16:])
	}
	for _, p := range part {
		if p.System == 0x0 {
			continue
		}
		fmt.Println(p)
	}

	return nil
}

func readPart(b []byte) Part {
	return Part{
		Bootable: b[0],
		Start: CHS{
			Head:     uint64(b[1]),
			Sector:   uint64(b[2]) & 0xbf,
			Cylinder: uint64(b[2])<<8 | uint64(b[3]),
		},
		System: PartID(b[4]),
		End: CHS{
			Head:     uint64(b[5]),
			Sector:   uint64(b[6]) & 0xbf,
			Cylinder: uint64(b[6])<<8 | uint64(b[7]),
		},
		Rel:   read32(b[8:]),
		Total: read32(b[12:]),
	}
}

func read32(b []byte) uint32 {
	return uint32(b[0]) | uint32(b[1])<<8 | uint32(b[2])<<16 | uint32(b[3])<<24
}

type CHS struct {
	Head     uint64
	Sector   uint64
	Cylinder uint64
}

type Part struct {
	Bootable uint8
	Start    CHS
	System   PartID
	End      CHS
	Rel      uint32
	Total    uint32
}

func (p Part) String() string {
	w := new(bytes.Buffer)
	fmt.Fprintf(w, "Bootable  | %#x\n", p.Bootable)
	fmt.Fprintf(w, "ID        | %v\n", p.System)
	fmt.Fprintf(w, "Start     | %d %d %d\n", p.Start.Cylinder, p.Start.Head, p.End.Sector)
	fmt.Fprintf(w, "End       | %d %d %d\n", p.End.Cylinder, p.End.Head, p.End.Sector)
	fmt.Fprintf(w, "Rel       | %d\n", p.Rel)
	fmt.Fprintf(w, "Sectors   | %d\n", p.Total)
	return w.String()
}

type PartID int

func (p PartID) String() string {
	switch p {
	case 0x00:
		return "Empty"
	case 0x01:
		return "DOS 12-bit FAT"
	case 0x02:
		return "XENIX root"
	case 0x03:
		return "XENIX /usr"
	case 0x04:
		return "DOS 3.0+ 16-bit FAT"
	case 0x05:
		return "DOS 3.3+ Extended Partition"
	case 0x06:
		return "DOS 3.31+ 16-bit FAT"
	case 0x07:
		return "HPFS/NTFS/ExFAT"
	case 0x0b:
		return "WIN95 OSR2 FAT32"
	case 0x39:
		return "Plan 9 partition"
	case 0xb1, 0xb2, 0xb3:
		return "HP Volume Expansion/QNX Neutrino Power-Safe"
	case 0x82:
		return "Linux swap"
	case 0x83:
		return "Linux"
	case 0xb7:
		return "BSDI BSD/386 filesystem"
	case 0xb8:
		return "BSDI BSD/386 swap partition"
	}
	return fmt.Sprintf("Unknown(%#x)", int(p))
}
