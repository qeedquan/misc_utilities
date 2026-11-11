// computes various checksums for data
package main

import (
	"crypto/hmac"
	"crypto/md5"
	"crypto/sha1"
	"crypto/sha256"
	"crypto/sha3"
	"crypto/sha512"
	"flag"
	"fmt"
	"hash"
	"hash/adler32"
	"hash/crc32"
	"hash/crc64"
	"hash/fnv"
	"hash/maphash"
	"io"
	"log"
	"os"
	"strings"

	"golang.org/x/crypto/blake2b"
	"golang.org/x/crypto/blake2s"
	"golang.org/x/crypto/md4"
	"golang.org/x/crypto/ripemd160"
)

var flags struct {
	hmac string
	off  int64
	size int64
}

func main() {
	log.SetFlags(0)

	flag.StringVar(&flags.hmac, "hmac", "", "use a hmac key")
	flag.Int64Var(&flags.off, "off", 0, "specify start data offset")
	flag.Int64Var(&flags.size, "size", 0, "specify data size")
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	h := makeHash(flag.Arg(0))
	if h == nil {
		log.Fatalf("unsupported checksum %q", flag.Arg(0))
	}

	if flags.hmac != "" {
		f := func() hash.Hash { return makeHash(flag.Arg(0)) }
		h = hmac.New(f, []byte(flags.hmac))
	}

	if flag.NArg() < 2 {
		sum(os.Stdin, "-", h)
	} else {
		for i := 1; i < flag.NArg(); i++ {
			name := flag.Arg(i)
			fi, err := os.Stat(name)
			if err != nil {
				fmt.Fprintln(os.Stderr, err)
				continue
			}

			filesize := fi.Size()
			off := flags.off
			size := flags.size
			if size == 0 {
				size = filesize
			}
			if off < 0 {
				off = filesize + off
			}
			if size < 0 {
				size = filesize + size
			}

			if off < 0 || off >= filesize || size < 0 || size > filesize {
				fmt.Fprintf(os.Stderr, "invalid offset/size (%d:%d - %d) %s\n", off, size, filesize, name)
				continue
			}

			f, err := os.Open(name)
			if err != nil {
				fmt.Fprintln(os.Stderr, err)
				continue
			}

			r := io.NewSectionReader(f, off, size)
			err = sum(r, name, h)
			if err != nil {
				fmt.Fprintln(os.Stderr, err)
			}

			f.Close()
			h.Reset()
		}
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] checksum file ...")
	flag.PrintDefaults()
	fmt.Fprintln(os.Stderr)
	fmt.Fprintln(os.Stderr, "supported checksums:")
	fmt.Fprintln(os.Stderr, "adler32 blake2b-256 blake2b-384 blake2b-512 blakes-256 crc8 crc16-ccitt crc32-ieee crc64-iso crc64-ecma fletch16 fnv32 fnv32a fnv64 fnv64a fnv128 fnv128a")
	fmt.Fprintln(os.Stderr, "lunh maphash md4 md5 ripemd160 sha1 sha256 sha384 sha3-224 sha3-256 sha3-384 sha3-512 sha512 sha512-224 sha512-256 sum16 sum32 sum64 xor8 zynqgemmac")
	os.Exit(2)
}

func mustMakeHash(h hash.Hash, err error) hash.Hash {
	if err != nil {
		log.Fatal("error making hash: ", err)
	}
	return h
}

func makeHash(name string) hash.Hash {
	switch strings.ToLower(name) {
	case "blake2b-256":
		return mustMakeHash(blake2b.New256(nil))
	case "blake2b-384":
		return mustMakeHash(blake2b.New384(nil))
	case "blake2b-512":
		return mustMakeHash(blake2b.New512(nil))
	case "blake2s-256":
		return mustMakeHash(blake2s.New256(nil))
	case "ripemd160":
		return ripemd160.New()
	case "maphash":
		return new(maphash.Hash)
	case "md4":
		return md4.New()
	case "md5":
		return md5.New()
	case "sha1":
		return sha1.New()
	case "sha256":
		return sha256.New()
	case "sha384":
		return sha512.New384()
	case "sha3-224":
		return sha3.New224()
	case "sha3-256":
		return sha3.New256()
	case "sha3-384":
		return sha3.New384()
	case "sha3-512":
		return sha3.New512()
	case "sha512":
		return sha512.New()
	case "sha512-224":
		return sha512.New512_224()
	case "sha512-256":
		return sha512.New512_256()
	case "crc32-ieee":
		return crc32.NewIEEE()
	case "crc64-iso":
		return crc64.New(crc64.MakeTable(crc64.ISO))
	case "crc64-ecma":
		return crc64.New(crc64.MakeTable(crc64.ECMA))
	case "adler32":
		return adler32.New()
	case "fnv32":
		return fnv.New32()
	case "fnv32a":
		return fnv.New32a()
	case "fnv64":
		return fnv.New64()
	case "fnv64a":
		return fnv.New64a()
	case "fnv128":
		return fnv.New128()
	case "fnv128a":
		return fnv.New128a()
	case "xor8":
		return new(xor8)
	case "fletch16":
		return &fletch16{}
	case "luhn":
		return new(luhn)
	case "sum16":
		return new(sum16)
	case "sum32":
		return new(sum32)
	case "sum64":
		return new(sum64)
	case "crc8":
		return new(crc8)
	case "crc16-ccitt":
		c := new(crc16ccitt)
		c.Reset()
		return c
	case "zynqgemmac":
		return new(zynqgemmac)
	}
	return nil
}

func sum(r io.Reader, name string, h hash.Hash) error {
	_, err := io.Copy(h, r)
	if err != nil {
		return err
	}
	digest := h.Sum(nil)
	fmt.Printf("%x %s\n", digest, name)
	return nil
}

type fletch16 struct {
	sum1, sum2 uint16
}

func (d *fletch16) Reset()      { d.sum1, d.sum2 = 0, 0 }
func (fletch16) Size() int      { return 2 }
func (fletch16) BlockSize() int { return 1 }

func (d *fletch16) Write(b []byte) (int, error) {
	for i := range b {
		d.sum1 = (d.sum1 + uint16(b[i])) % 255
		d.sum2 = (d.sum2 + d.sum1) % 255
	}
	return len(b), nil
}

func (d *fletch16) Sum(b []byte) []byte {
	return append(b, byte(d.sum2), byte(d.sum1))
}

type xor8 uint8

func (d *xor8) Reset()      { *d = 0 }
func (xor8) Size() int      { return 1 }
func (xor8) BlockSize() int { return 1 }

func (d *xor8) Write(b []byte) (int, error) {
	for i := range b {
		*d ^= xor8(b[i])
	}
	return len(b), nil
}

func (d xor8) Sum(b []byte) []byte {
	return append(b, uint8(d))
}

type luhn uint64

func (d *luhn) Reset()      { *d = 0 }
func (luhn) Size() int      { return 8 }
func (luhn) BlockSize() int { return 1 }

func (d *luhn) Write(b []byte) (int, error) {
	tab := [...]uint64{0, 2, 4, 6, 8, 1, 3, 5, 7, 9}
	odd := len(b) & 1

	var sum uint64
	for i, c := range b {
		if c < '0' || c > '9' {
			c %= 10
		} else {
			c -= '0'
		}
		if i&1 == odd {
			sum += tab[c]
		} else {
			sum += uint64(c)
		}
	}
	*d = luhn(sum)

	return len(b), nil
}

func (d luhn) Sum(b []byte) []byte {
	return append(b, byte(d), byte(d>>8), byte(d>>16), byte(d>>24),
		byte(d>>32), byte(d>>40), byte(d>>48), byte(d>>56))
}

type sum16 uint16

func (d *sum16) Reset()      { *d = 0 }
func (sum16) Size() int      { return 2 }
func (sum16) BlockSize() int { return 1 }

func (d *sum16) Write(b []byte) (int, error) {
	for i := range b {
		*d += sum16(b[i])
	}
	return len(b), nil
}

func (d sum16) Sum(b []byte) []byte {
	return append(b, byte(d&0xff), byte(d>>8))
}

type sum32 uint32

func (d *sum32) Reset()      { *d = 0 }
func (sum32) Size() int      { return 4 }
func (sum32) BlockSize() int { return 1 }

func (d *sum32) Write(b []byte) (int, error) {
	for i := range b {
		*d += sum32(b[i])
	}
	return len(b), nil
}

func (d sum32) Sum(b []byte) []byte {
	return append(b, byte(d&0xff), byte(d>>8), byte(d>>16), byte(d>>24))
}

type sum64 uint64

func (d *sum64) Reset()      { *d = 0 }
func (sum64) Size() int      { return 8 }
func (sum64) BlockSize() int { return 1 }

func (d *sum64) Write(b []byte) (int, error) {
	for i := range b {
		*d += sum64(b[i])
	}
	return len(b), nil
}

func (d sum64) Sum(b []byte) []byte {
	return append(b, byte(d&0xff), byte(d>>8), byte(d>>16), byte(d>>24), byte(d>>32), byte(d>>40), byte(d>>48), byte(d>>56))
}

type crc8 uint8

func (c *crc8) Reset()      { *c = 0 }
func (crc8) Size() int      { return 1 }
func (crc8) BlockSize() int { return 1 }

func (c crc8) calc(b byte) crc8 {
	d := c ^ crc8(b)
	for i := 0; i < 8; i++ {
		if d&0x80 != 0 {
			d <<= 1
			d ^= 0x07
		} else {
			d <<= 1
		}
	}
	return d
}

func (c *crc8) Write(b []byte) (int, error) {
	for i := range b {
		*c = c.calc(b[i])
	}
	return len(b), nil
}

func (c crc8) Sum(b []byte) []byte {
	return append(b, byte(c))
}

type crc16ccitt uint16

func (c *crc16ccitt) Reset()      { *c = 0xffff }
func (crc16ccitt) Size() int      { return 2 }
func (crc16ccitt) BlockSize() int { return 1 }

func (c *crc16ccitt) Write(b []byte) (int, error) {
	for i := range b {
		x := *c>>8 ^ crc16ccitt(b[i])
		x ^= x >> 4
		*c = (*c << 8) ^ crc16ccitt((x<<12)^(x<<5)^x)
	}
	return len(b), nil
}

func (c crc16ccitt) Sum(b []byte) []byte {
	return append(b, uint8(c>>8), uint8(c))
}

type zynqgemmac struct {
	v [6]byte
}

func (c *zynqgemmac) Reset()         { c.v = [6]byte{} }
func (c *zynqgemmac) Size() int      { return 4 }
func (c *zynqgemmac) BlockSize() int { return 1 }

func (c *zynqgemmac) Write(b []byte) (int, error) {
	var p [6]byte
	for i := 0; i < len(b); i += 6 {
		copy(p[:], b[i:])
		c.v[5] += ((p[0] >> 5) ^ (p[1] >> 3) ^ (p[2] >> 1) ^ (p[2] >> 7) ^ (p[3] >> 5) ^ (p[4] >> 3) ^ (p[5] >> 1) ^ (p[5] >> 7)) & 1
		c.v[4] += ((p[0] >> 4) ^ (p[1] >> 2) ^ (p[2]) ^ (p[2] >> 6) ^ (p[3] >> 4) ^ (p[4] >> 2) ^ (p[5]) ^ (p[5] >> 6)) & 1
		c.v[3] += ((p[0] >> 3) ^ (p[1] >> 1) ^ (p[1] >> 7) ^ (p[2] >> 5) ^ (p[3] >> 3) ^ (p[4] >> 1) ^ (p[4] >> 7) ^ (p[5] >> 5)) & 1
		c.v[2] += ((p[0] >> 2) ^ (p[1]) ^ (p[1] >> 6) ^ (p[2] >> 4) ^ (p[3] >> 2) ^ (p[4]) ^ (p[4] >> 6) ^ (p[5] >> 4)) & 1
		c.v[1] += ((p[0] >> 1) ^ (p[0] >> 7) ^ (p[1] >> 5) ^ (p[2] >> 3) ^ (p[3] >> 1) ^ (p[3] >> 7) ^ (p[4] >> 5) ^ (p[5] >> 3)) & 1
		c.v[0] += ((p[0]) ^ (p[0] >> 6) ^ (p[1] >> 4) ^ (p[2] >> 2) ^ (p[3]) ^ (p[3] >> 6) ^ (p[4] >> 4) ^ (p[5] >> 2)) & 1
	}
	return len(b), nil
}

func (c *zynqgemmac) Sum(b []byte) []byte {
	h := uint32(0)
	for i := range c.v {
		if c.v[i] != 0 {
			h += uint32(1 << i)
		}
	}
	return append(b, uint8(h>>24), uint8(h>>16), uint8(h>>8), uint8(h))
}
