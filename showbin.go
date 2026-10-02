package main

import (
	"bufio"
	"encoding/binary"
	"flag"
	"fmt"
	"io"
	"log"
	"os"
	"reflect"
)

var (
	status int
)

var flags struct {
	typ    string
	endian string
	off    int64
	size   int64
}

func main() {
	log.SetFlags(0)
	log.SetPrefix("showbin: ")
	parseflags()
	if flag.NArg() == 0 {
		ck(dump(os.Stdin, flags.typ, flags.endian, flags.off, flags.size))
	} else {
		for _, name := range flag.Args() {
			f, err := os.Open(name)
			if ek(err) {
				continue
			}

			err = dump(f, flags.typ, flags.endian, flags.off, flags.size)
			ek(err)

			f.Close()
		}
	}
	os.Exit(status)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func ek(err error) bool {
	if err != nil {
		fmt.Fprintln(os.Stderr, "showbin: ", err)
		status = 1
		return true
	}
	return false
}

func parseflags() {
	flag.StringVar(&flags.typ, "t", "u8", "specify binary type")
	flag.StringVar(&flags.endian, "e", "le", "specify binary endian")
	flag.Int64Var(&flags.off, "o", 0, "show at offset")
	flag.Int64Var(&flags.size, "s", 0, "show up to size bytes")
	flag.Usage = usage
	flag.Parse()
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] <file> ...")
	flag.PrintDefaults()
	fmt.Fprintln(os.Stderr, "available endian: le be")
	fmt.Fprintln(os.Stderr, "available types: u8 u16 u32 u64 f32 f64 c64 c128")
	os.Exit(2)
}

func dump(r io.Reader, typ, endian string, off, size int64) (err error) {
	err = discard(r, off)
	if err != nil {
		return
	}

	var order binary.ByteOrder
	switch endian {
	case "le":
		order = binary.LittleEndian
	case "be":
		order = binary.BigEndian
	default:
		err = fmt.Errorf("invalid binary endian")
		return
	}

	b := bufio.NewReader(r)
	n := int64(0)
	for i := uint64(1); ; i++ {
		if size > 0 && n >= size {
			break
		}

		var (
			v interface{}
			f string
			e int
		)
		v, f, e, err = read(b, typ, order)
		if err == io.EOF {
			err = nil
			break
		}
		if err != nil {
			break
		}

		f = fmt.Sprintf("%%-8d %s\n", f)
		switch e {
		case 1:
			fmt.Printf(f, i, v)
		case 3:
			fmt.Printf(f, i, v, v, v)
		}
		rt := reflect.TypeOf(v)
		n += int64(rt.Size())
	}

	return
}

func discard(r io.Reader, n int64) (err error) {
	s, _ := r.(io.Seeker)
	if s != nil && s != os.Stdin {
		_, err = s.Seek(n, io.SeekStart)
	} else {
		_, err = io.CopyN(io.Discard, r, n)
	}
	return
}

func read(r io.Reader, typ string, order binary.ByteOrder) (v interface{}, f string, e int, err error) {
	tab := []struct {
		name   string
		format string
		elem   int
		value  interface{}
	}{
		{"u8", "%2X %3d %08b", 3, new(uint8)},
		{"u16", "%4X %5d %016b", 3, new(uint16)},
		{"u32", "%8X %10d %032b", 3, new(uint32)},
		{"u64", "%16X %20d %064b", 3, new(uint64)},
		{"f32", "%.6f", 1, new(float32)},
		{"f64", "%.6f", 1, new(float64)},
		{"c64", "%.6f", 1, new(complex64)},
		{"c128", "%.6f", 1, new(complex128)},
	}

	for _, p := range tab {
		if typ == p.name {
			err = binary.Read(r, order, p.value)
			if err != nil {
				return
			}

			rv := reflect.ValueOf(p.value)
			iv := reflect.Indirect(rv)
			v = iv.Interface()
			f = p.format
			e = p.elem
			return
		}
	}

	err = fmt.Errorf("invalid type %q", typ)
	return
}
