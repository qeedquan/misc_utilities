// +build linux

// ported from suckless
package main

import (
	"flag"
	"fmt"
	"io"
	"log"
	"os"
	"strings"
	"syscall"
)

const (
	SWAP_MAGIC1       = "SWAPSPACE2"
	SWAP_MAGIC2       = "SWAP-SPACE"
	SWAP_MAGIC_LENGTH = 10
	SWAP_LABEL_LENGTH = 16
	SWAP_LABEL_OFFSET = 1024 + 4 + 4 + 4 + 16
)

var (
	SWAP_MAGIC_OFFSET = int64(syscall.Getpagesize()) - SWAP_MAGIC_LENGTH
)

var (
	label = flag.String("L", "", "set label")
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("swaplabel: ")

	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}
	device := flag.Arg(0)

	f, err := os.OpenFile(device, os.O_RDWR, 0644)
	ck(err)
	defer f.Close()

	_, err = f.Seek(SWAP_MAGIC_OFFSET, io.SeekStart)
	ck(err)

	var magic [SWAP_MAGIC_LENGTH]byte
	_, err = io.ReadAtLeast(f, magic[:], len(magic))
	ck(err)

	if string(magic[:]) != SWAP_MAGIC1 && string(magic[:]) != SWAP_MAGIC2 {
		log.Fatalf("%s: is not a swap partition", device)
	}

	_, err = f.Seek(SWAP_LABEL_OFFSET, io.SeekStart)
	ck(err)

	if *label == "" {
		var buf [SWAP_LABEL_LENGTH]byte
		_, err = io.ReadAtLeast(f, buf[:], len(buf))
		ck(err)

		for i := range buf {
			if i == SWAP_LABEL_LENGTH-1 && buf[i] != 0 {
				log.Fatal("invalid label")
			}
		}
		fmt.Println(strings.TrimRight(string(buf[:]), "\x00"))
	} else {
		if len(*label)+1 > SWAP_LABEL_LENGTH {
			log.Fatal("label too long")
		}
		_, err = f.Write([]byte(*label + "\x00"))
		ck(err)
	}

	syscall.Fsync(int(f.Fd()))
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: swaplabel [-L] device")
	flag.PrintDefaults()
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}
