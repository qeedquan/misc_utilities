package main

import (
	"bufio"
	"crypto/dsa"
	"crypto/md5"
	"crypto/rand"
	"crypto/sha1"
	"crypto/sha256"
	"crypto/sha3"
	"crypto/sha512"
	"encoding/hex"
	"flag"
	"fmt"
	"hash"
	"hash/adler32"
	"hash/crc32"
	"hash/crc64"
	"hash/fnv"
	"io"
	"log"
	"math/big"
	"os"
	"strings"
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("dsa: ")

	var h, c string
	var p bool
	flag.Usage = usage
	flag.StringVar(&h, "h", "sha512", "use hash")
	flag.StringVar(&c, "c", "L3072N256", "use parameter")
	flag.BoolVar(&p, "p", true, "print private keys/hash when signing")
	flag.Bool("s", false, "sign file")
	flag.Bool("v", false, "verify file")
	flag.Parse()

	var mode uint
	flag.Visit(func(f *flag.Flag) {
		switch f.Name {
		case "s":
			mode |= 1
		case "v":
			mode |= 2
		}
	})

	switch mode {
	case 1:
		if flag.NArg() < 1 {
			usage()
		}
		ck(sign(flag.Arg(0), p, makeHash(h), makeParams(c)))
	case 2:
		if flag.NArg() < 2 {
			usage()
		}
		ck(verify(flag.Arg(0), flag.Arg(1), makeHash(h), makeParams(c)))
	default:
		usage()
	}

}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: dsa [options] -s <file>")
	fmt.Fprintln(os.Stderr, "       dsa [options] -v <key> <file>")
	flag.PrintDefaults()
	fmt.Fprintln(os.Stderr, "")
	fmt.Fprintln(os.Stderr, "valid parameters: L1024N160 L2048N224 L2048N256 L3072N256")
	fmt.Fprintln(os.Stderr, "valid hash: md5 sha1 sha256 sha384 sha512 sha512-224 sha512-256 sha3-224 sha3-256 sha3-384 sha3-512")
	fmt.Fprintln(os.Stderr, "            crc32-ieee crc64-iso crc64-ecma adler32 fnv32 fnv32a fnv64 fnv64a")

	os.Exit(2)
}

func sign(name string, priv bool, h hash.Hash, c *dsa.Parameters) error {
	m, err := sum(name, h)
	if err != nil {
		return err
	}

	p := &dsa.PrivateKey{}
	p.Parameters = *c
	err = dsa.GenerateKey(p, rand.Reader)
	if err != nil {
		return err
	}

	r, s, err := dsa.Sign(rand.Reader, p, m)
	if err != nil {
		return err
	}

	w := os.Stdout
	if priv {
		fmt.Fprintf(w, "x=%v\n", p.X)
		fmt.Fprintf(w, "h=%v\n", hex.EncodeToString(m))
	}
	fmt.Fprintf(w, "y=%v\n", p.Y)
	fmt.Fprintf(w, "p=%v\n", p.P)
	fmt.Fprintf(w, "q=%v\n", p.Q)
	fmt.Fprintf(w, "g=%v\n", p.G)
	fmt.Fprintf(w, "r=%v\n", r)
	fmt.Fprintf(w, "s=%v\n", s)
	return nil
}

func verify(key, name string, h hash.Hash, c *dsa.Parameters) error {
	m, err := sum(name, h)
	if err != nil {
		return err
	}

	f, err := os.Open(key)
	if err != nil {
		return err
	}
	defer f.Close()

	k := &dsa.PublicKey{}
	k.P = new(big.Int)
	k.Q = new(big.Int)
	k.G = new(big.Int)
	k.Y = new(big.Int)
	R := new(big.Int)
	S := new(big.Int)

	s := bufio.NewScanner(f)
	for s.Scan() {
		t := s.Text()
		n, _ := fmt.Sscanf(t, "p=%v", k.P)
		if n == 1 {
			continue
		}
		n, _ = fmt.Sscanf(t, "q=%v", k.Q)
		if n == 1 {
			continue
		}
		n, _ = fmt.Sscanf(t, "g=%v", k.G)
		if n == 1 {
			continue
		}
		n, _ = fmt.Sscanf(t, "y=%v", k.Y)
		if n == 1 {
			continue
		}
		n, _ = fmt.Sscanf(t, "r=%v", R)
		if n == 1 {
			continue
		}
		fmt.Sscanf(t, "s=%v", S)
	}

	v := dsa.Verify(k, m, R, S)
	if !v {
		fmt.Printf("DSA verification failed for file %q\n", name)
	} else {
		fmt.Printf("DSA verification succeeded for file %q\n", name)
	}

	return nil
}

func sum(name string, h hash.Hash) ([]byte, error) {
	f, err := os.Open(name)
	if err != nil {
		return nil, err
	}
	defer f.Close()
	b := bufio.NewReader(f)
	io.Copy(h, b)

	m := h.Sum(nil)
	return m, nil
}

func makeHash(name string) hash.Hash {
	switch strings.ToLower(name) {
	case "md5":
		return md5.New()
	case "sha1":
		return sha1.New()
	case "sha256":
		return sha256.New()
	case "sha384":
		return sha512.New384()
	case "sha512":
		return sha512.New()
	case "sha512-224":
		return sha512.New512_224()
	case "sha512-256":
		return sha512.New512_256()
	case "sha3-224":
		return sha3.New224()
	case "sha3-256":
		return sha3.New256()
	case "sha3-384":
		return sha3.New384()
	case "sha3-512":
		return sha3.New512()
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
	}
	log.Fatalf("unknown hash %q", name)
	return nil
}

func makeParams(name string) *dsa.Parameters {
	var err error
	p := &dsa.Parameters{}
	switch name {
	case "L1024N160":
		err = dsa.GenerateParameters(p, rand.Reader, dsa.L1024N160)
	case "L2048N224":
		err = dsa.GenerateParameters(p, rand.Reader, dsa.L2048N224)
	case "L2048N256":
		err = dsa.GenerateParameters(p, rand.Reader, dsa.L2048N256)
	case "L3072N256":
		err = dsa.GenerateParameters(p, rand.Reader, dsa.L3072N256)
	default:
		log.Fatalf("unknown parameters %q", name)
		return nil
	}
	if err != nil {
		log.Fatalf("error generate parameters: %v", err)
	}
	return p
}
