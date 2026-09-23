package main

import (
	"encoding/hex"
	"flag"
	"fmt"
	"log"
	"math"
	"os"
	"strings"
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("vaxfp: ")
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}
	s := strings.Join(flag.Args(), " ")
	b, err := hex.DecodeString(s)
	if err != nil {
		log.Fatal(err)
	}

	var p [4]byte
	copy(p[:], b)
	fmt.Printf("%f\n", b2f(p[:]))
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: vaxfp num")
	flag.PrintDefaults()
	os.Exit(2)
}

func b2f(b []byte) float64 {
	s, e, f := b2sef(b)
	return sef2f(s, e, f)
}

func b2sef(b []byte) (s, e, f uint) {
	b1 := b[1]
	b2 := b[0]
	b3 := b[3]
	b4 := b[2]

	s = uint(b1&0x80) >> 7
	e = uint(b1&0x7F)<<1 + uint(b2&0x80)>>7
	f = uint((b2&0x7F))<<16 + uint(b3)<<8 + uint(b4)
	return
}

func sef2f(s, e, f uint) float64 {
	a := 2.0
	b := 128.0
	c := 0.5
	e24 := math.Pow(2, 24)
	m := float64(f) / e24
	f1 := 1.0
	if s != 0 {
		f1 = -1
	}
	if e >= 0 {
		return f1 * (c + m) * math.Pow(a, float64(e)-b)
	} else if e == 0 && s == 0 {
		return 0
	} else if e == 0 && s == 1 {
		return -0
	}
	return 0
}
