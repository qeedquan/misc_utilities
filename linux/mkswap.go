// ported from toybox

package main

import (
	"crypto/rand"
	"flag"
	"fmt"
	"io"
	"os"
	"syscall"
)

var (
	label = flag.String("L", "", "set swap label")
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() != 1 {
		usage()
	}

	f, err := os.OpenFile(flag.Arg(0), os.O_RDWR, 0644)
	ck(err)
	defer f.Close()

	fi, err := f.Stat()
	ck(err)

	pagesize := syscall.Getpagesize()
	pages := fi.Size()/int64(pagesize) - 1

	var swap [8192]byte
	put4(swap[0:], 1)
	put4(swap[4:], uint32(pages))
	seek(f, 1024)
	uuid := createUUID()
	copy(swap[4*3:], uuid)
	if *label != "" {
		i := 0
		for ; i < len(*label) && i < 16; i++ {
			swap[7*4+i] = (*label)[i]
		}
		if i < 16 {
			swap[7*4+i] = 0
		}
	}
	write(f, swap[:129*4])
	seek(f, int64(pagesize)-10)
	write(f, []byte("SWAPSPACE2"))

	syscall.Fsync(int(f.Fd()))
	f.Close()

	var str string
	if *label != "" {
		str = fmt.Sprintf(", LABEL=%s", *label)
	}
	fmt.Printf("Swapspace size: %dk%s, UUID=%s\n",
		pages*(int64(pagesize)/1024), str, showUUID(uuid))
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: mkswap [-L label] device")
	flag.PrintDefaults()
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		fmt.Fprintln(os.Stderr, "mkswap:", err)
		os.Exit(1)
	}
}

func put4(b []byte, v uint32) {
	b[0] = uint8(v)
	b[1] = uint8(v >> 8)
	b[2] = uint8(v >> 16)
	b[3] = uint8(v >> 24)
}

func seek(f *os.File, off int64) {
	_, err := f.Seek(off, io.SeekStart)
	ck(err)
}

func write(f *os.File, buf []byte) int {
	n, err := f.Write(buf)
	ck(err)
	return n
}

func createUUID() []byte {
	var uuid [16]byte
	_, err := rand.Read(uuid[:])
	ck(err)

	uuid[6] = (uuid[6] & 0xf) | 0x40
	uuid[8] = (uuid[8] & 0x3f) | 0x80
	uuid[11] |= 128
	return uuid[:]
}

func showUUID(uuid []byte) string {
	var out string
	for i := uint(0); i < 16; i++ {
		if 0x550&(1<<i) == 0 {
			out += fmt.Sprintf("%02x", uuid[i])
		} else {
			out += fmt.Sprintf("-%02x", uuid[i])
		}
	}
	return out
}
