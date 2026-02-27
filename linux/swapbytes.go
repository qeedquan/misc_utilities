package main

import (
	"bufio"
	"encoding/binary"
	"flag"
	"fmt"
	"io"
	"log"
	"os"
)

var (
	nbs = flag.Int("n", 2, "swap every N bytes")
	pad = flag.Uint("p", 0, "pad with value")
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("swapbytes: ")
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() != 2 {
		usage()
	}
	swapbytes(flag.Arg(0), flag.Arg(1))
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: swapbytes [options] infile outfile")
	flag.PrintDefaults()
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func swapbytes(infile, outfile string) {
	ifp, err := os.Open(infile)
	ck(err)

	ofp, err := os.Create(outfile)
	ck(err)

	r := bufio.NewReader(ifp)
	w := bufio.NewWriter(ofp)
	n := *nbs
	if !(n == 1 || n == 2 || n == 4 || n == 8) {
		log.Fatalf("unsupported swap of every %d bytes", n)
	}

	var b [8]byte
	for {
		for i := range b {
			b[i] = uint8(*pad)
		}
		_, err := r.Read(b[:n])
		if err == io.EOF {
			break
		}
		ck(err)
		swap(b[:n])
		w.Write(b[:n])
	}

	ck(w.Flush())
	ck(ofp.Close())
}

func swap(b []byte) {
	switch len(b) {
	case 2:
		v := binary.LittleEndian.Uint16(b)
		binary.BigEndian.PutUint16(b, v)
	case 4:
		v := binary.LittleEndian.Uint32(b)
		binary.BigEndian.PutUint32(b, v)
	case 8:
		v := binary.LittleEndian.Uint64(b)
		binary.BigEndian.PutUint64(b, v)
	}
}
