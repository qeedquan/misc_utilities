package main

import (
	"crypto/md5"
	"crypto/sha1"
	"crypto/sha256"
	"crypto/sha3"
	"crypto/sha512"
	"flag"
	"fmt"
	"hash"
	"io"
	"log"
	"os"
	"strconv"

	"golang.org/x/crypto/hkdf"
	"golang.org/x/crypto/pbkdf2"
)

var flags struct {
	alg    string
	hash   string
	secret string
	salt   string
	info   string
	iter   int
}

func main() {
	log.SetFlags(0)
	log.SetPrefix("kdf: ")

	parseflags()

	var out, salt, info []byte
	if flags.salt != "" {
		salt = []byte(flags.salt)
	}
	if flags.info != "" {
		info = []byte(flags.info)
	}

	secret, err := os.ReadFile(flag.Arg(0))
	check(err)

	keylen, err := strconv.Atoi(flag.Arg(1))
	check(err)

	h := gethash(flags.hash)
	switch flags.alg {
	case "hkdf":
		r := hkdf.New(h, secret, salt, info)
		out = make([]byte, keylen)
		_, err = io.ReadAtLeast(r, out, keylen)
		check(err)
	case "pbkdf2":
		out = pbkdf2.Key(secret, salt, flags.iter, keylen, h)
	}
	fmt.Printf("%x\n", out)
}

func parseflags() {
	flag.Usage = usage
	flag.StringVar(&flags.alg, "alg", "hkdf", "kdf algorithm to use")
	flag.StringVar(&flags.hash, "hash", "sha512", "hash algorithm to use")
	flag.StringVar(&flags.salt, "salt", "salt", "specify salt")
	flag.StringVar(&flags.info, "info", "label", "specify info [hkdf]")
	flag.IntVar(&flags.iter, "iter", 4096, "specify iterations [pbkdf2]")
	flag.Parse()
	if flag.NArg() != 2 {
		usage()
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] <secret_file> <keylen>")
	flag.PrintDefaults()
	fmt.Fprintln(os.Stderr, "available algorithms: hkdf pbkdf2")
	fmt.Fprintln(os.Stderr, "available hashes: md5 sha1 sha224 sha256 sha384 sha512-224 sha512-256 sha512 sha3-224 sha3-256 sha3-384 sha3-512")
	os.Exit(2)
}

func check(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func gethash(name string) func() hash.Hash {
	switch name {
	case "md5":
		return md5.New
	case "sha1":
		return sha1.New
	case "sha224":
		return sha256.New224
	case "sha256":
		return sha256.New
	case "sha384":
		return sha512.New384
	case "sha512-224":
		return sha512.New512_224
	case "sha512-256":
		return sha512.New512_256
	case "sha512":
		return sha512.New
	case "sha3-224":
		return func() hash.Hash { return sha3.New224() }
	case "sha3-256":
		return func() hash.Hash { return sha3.New256() }
	case "sha3-384":
		return func() hash.Hash { return sha3.New384() }
	case "sha3-512":
		return func() hash.Hash { return sha3.New512() }
	}

	log.Fatalf("unknown hash %q", name)
	return nil
}
