// https://en.wikipedia.org/wiki/Tiny_Encryption_Algorithm
package main

import (
	"encoding/binary"
	"flag"
	"fmt"
	"log"
	"os"
)

var (
	endian = flag.String("i", "little", "specify endian order")
	openc  = flag.Bool("e", false, "encrypt input")
	opdec  = flag.Bool("d", false, "decrypt input")
)

func main() {
	log.SetPrefix("tea: ")
	log.SetFlags(0)
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() != 3 {
		usage()
	}

	var order binary.ByteOrder
	if *endian == "little" {
		order = binary.LittleEndian
	} else {
		order = binary.BigEndian
	}
	key := convert([]byte(flag.Arg(0)), order)
	if len(key) != 4 {
		log.Fatal("invalid key length")
	}

	buf, err := os.ReadFile(flag.Arg(1))
	ck(err)

	data := convert(buf, order)
	var out32 []uint32
	if *openc {
		out32 = docrypt(key, data, 'e')
	} else if *opdec {
		out32 = docrypt(key, data, 'd')
	} else {
		usage()
	}

	out := deconvert(out32)
	ck(os.WriteFile(flag.Arg(2), out, 0644))
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: key infile outfile")
	flag.PrintDefaults()
	os.Exit(2)
}

func docrypt(key, data []uint32, op int) []uint32 {
	var p []uint32
	for i := 0; i < len(data); i += 2 {
		var v [2]uint32

		v[0] = data[0]
		v[1] = data[1]
		if op == 'e' {
			encrypt(v[:], key)
		} else {
			decrypt(v[:], key)
		}
		p = append(p, v[:]...)
	}
	return p
}

func deconvert(b []uint32) []byte {
	var p []byte
	var o [4]byte
	for i := range b {
		binary.LittleEndian.PutUint32(o[:], b[i])
		p = append(p, o[:]...)
	}
	return p
}

func convert(b []byte, e binary.ByteOrder) []uint32 {
	var p []uint32
	for i := 0; i < len(b)/4; i++ {
		p = append(p, e.Uint32(b[4*i:]))
	}

	n := len(b) % 4
	if n != 0 {
		var m [4]byte
		copy(m[:], b[:len(b)-n])
		p = append(p, e.Uint32(m[:]))
	}

	if len(p)%2 != 0 {
		p = append(p, 0)
	}
	return p
}

func encrypt(v, k []uint32) {
	v0 := v[0]
	v1 := v[1]
	sum := uint32(0)
	delta := uint32(0x9e3779b9)
	k0 := k[0]
	k1 := k[1]
	k2 := k[2]
	k3 := k[3]
	for i := 0; i < 32; i++ {
		sum += delta
		v0 += ((v1 << 4) + k0) ^ (v1 + sum) ^ ((v1 >> 5) + k1)
		v1 += ((v0 << 4) + k2) ^ (v0 + sum) ^ ((v0 >> 5) + k3)
	}
	v[0] = v0
	v[1] = v1
}

func decrypt(v, k []uint32) {
	v0 := v[0]
	v1 := v[1]
	sum := uint32(0xC6EF3720)
	delta := uint32(0x9e3779b9)
	k0 := k[0]
	k1 := k[1]
	k2 := k[2]
	k3 := k[3]
	for i := 0; i < 32; i++ {
		v1 -= ((v0 << 4) + k2) ^ (v0 + sum) ^ ((v0 >> 5) + k3)
		v0 -= ((v1 << 4) + k0) ^ (v1 + sum) ^ ((v1 >> 5) + k1)
		sum -= delta
	}
	v[0] = v0
	v[1] = v1
}
