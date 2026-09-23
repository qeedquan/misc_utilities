// ported from linux ihex2fw
package main

import (
	"bufio"
	"bytes"
	"encoding/binary"
	"flag"
	"fmt"
	"io"
	"log"
	"os"
)

func main() {
	log.SetFlags(0)

	var opt options
	flag.BoolVar(&opt.wide, "w", false, "wide records (16-bit length)")
	flag.BoolVar(&opt.sort, "s", false, "sort records by address")
	flag.BoolVar(&opt.jump, "j", false, "include records for CS:IP/EIP address")
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() != 2 {
		usage()
	}

	r, err := decode(flag.Arg(0), opt)
	ck(err)

	err = output(r, flag.Arg(1))
	ck(err)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] <src.hex> <dst.bin>")
	flag.PrintDefaults()
	os.Exit(1)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func output(r *record, name string) error {
	f, err := os.Create(name)
	if err != nil {
		return err
	}

	w := bufio.NewWriter(f)
	b := binWriter{w: w}
	for p := r; p != nil; p = p.next {
		b.Write(p.addr)
		b.Write(uint16(len(p.data)))
		b.Write(p.data)

		size := 4 + 2 + len(p.data)
		pad := (size+3)&^3 - size
		for i := 0; i < pad; i++ {
			b.Write(uint8(0))
		}
	}
	b.Write([6]byte{})

	err = w.Flush()
	xerr := f.Close()
	if err != nil {
		return err
	}
	return xerr
}

type binWriter struct {
	w io.Writer
}

func (b *binWriter) Write(x interface{}) {
	binary.Write(b.w, binary.BigEndian, x)
}

type options struct {
	wide bool
	sort bool
	jump bool
}

type decoder struct {
	options
	r       *bytes.Reader
	data    []byte
	line    uint64
	offset  uint32
	crc     byte
	records *record
}

type record struct {
	typ  uint8
	addr uint32
	data []byte
	next *record
}

func nybble(n byte) byte {
	switch {
	case '0' <= n && n <= '9':
		return n - '0'
	case 'A' <= n && n <= 'F':
		return n - ('A' - 10)
	case 'a' <= n && n <= 'f':
		return n - ('a' - 10)
	}
	return 0
}

func hex(b []byte, crc *byte) byte {
	v := nybble(b[0])<<4 | nybble(b[1])
	*crc += v
	return v
}

func swap32(b []byte) {
	b[0], b[3] = b[3], b[0]
	b[1], b[2] = b[2], b[1]
}

func decode(name string, opt options) (*record, error) {
	data, err := os.ReadFile(name)
	if err != nil {
		return nil, err
	}

	d := decoder{
		options: opt,
		r:       bytes.NewReader(data),
		data:    data,
		line:    1,
	}

	for {
		r, err := d.next()
		if err != nil {
			return nil, err
		}

		err = d.process(r)
		if err == io.EOF {
			break
		}
		if err != nil {
			return nil, err
		}
	}

	return d.records, nil
}

func (d *decoder) next() (*record, error) {
	for {
		c, err := d.r.ReadByte()
		if err == io.EOF {
			break
		}

		if err != nil {
			return nil, err
		}

		if c == '\n' {
			d.line++
		} else if c == ':' {
			break
		}
	}

	if d.r.Len() < 10 {
		return nil, fmt.Errorf("can't find valid record at line %v", d.line)
	}

	n := uint16(0)
	if d.wide {
		n = d.read16()
	} else {
		n = uint16(d.read8())
	}

	if 8+int(n)*2 > d.r.Len() {
		return nil, fmt.Errorf("not enough data to read complete record at line %v", d.line)
	}

	r := &record{}
	r.data = make([]byte, n)
	r.addr = uint32(d.read16())
	r.typ = d.read8()
	for i := range r.data {
		r.data[i] = d.read8()
	}

	crcByte := d.read8()
	if d.crc != 0 {
		return nil, fmt.Errorf("CRC failure at line %v: got %#x, expected %#x", d.line, crcByte, byte(crcByte-d.crc))
	}

	return r, nil
}

func (d *decoder) process(r *record) error {
	switch r.typ {
	case 0: // old style EOF record?
		if len(r.data) == 0 {
			break
		}
		r.addr += d.offset
		d.record(r)

	case 1: // EOF record
		if r.addr != 0 || len(r.data) != 0 {
			return fmt.Errorf("bad EOF record (type 01) format at line %v", d.line)
		}
		return io.EOF

	case 2: // Extended Segment Address Record (HEX86)
		fallthrough
	case 4: // Extended Linear Address Record (HEX386)
		if r.addr != 0 || len(r.data) != 2 {
			return fmt.Errorf("bad hex86/hex386	record (type %02x) at line %v", r.typ, d.line)
		}

		// shouldn't really be using offset for hex86 because
		// the wraparound case is specified quite differently
		d.offset = uint32(r.data[0])<<8 | uint32(r.data[1])
		if r.typ == 2 {
			d.offset <<= 4
		} else {
			d.offset <<= 16
		}

	case 3: // Start Segment Address record
		fallthrough
	case 5: // Start Linear Address Record
		if r.addr != 0 || len(r.data) != 4 {
			return fmt.Errorf("bad start address record (type %02x) at line %v", r.typ, d.line)
		}

		// these records contain the CS/IP or EIP where execution starts
		// if requested, output these as a record
		if d.jump {
			swap32(r.data)
			d.record(r)
		}

	default:
		return fmt.Errorf("unknown record type (type %02x)", r.typ)
	}

	return nil
}

func (d *decoder) record(r *record) {
	p := &d.records
	for *p != nil && (!d.sort || (*p).addr < r.addr) {
		p = &((*p).next)
	}
	r.next = *p
	*p = r
}

func (d *decoder) read8() byte {
	var b [2]byte
	b[0], _ = d.r.ReadByte()
	b[1], _ = d.r.ReadByte()
	return hex(b[:], &d.crc)
}

func (d *decoder) read16() uint16 {
	hi := uint16(d.read8())
	lo := uint16(d.read8())
	return hi<<8 | lo
}
