package main

import (
	"crypto/aes"
	"crypto/cipher"
	"crypto/des"
	"encoding/hex"
	"flag"
	"fmt"
	"io"
	"log"
	"os"
)

var (
	status = 0

	cipher_ = flag.String("c", "aes", "cipher type")
	mode    = flag.String("m", "gcm", "mode")
	key     = flag.String("k", "", "key")
	nonce   = flag.String("n", "", "nonce")
	enc     = flag.Bool("e", false, "encrypt input")
	dec     = flag.Bool("d", false, "decrypt input")
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("aead: ")

	flag.Usage = usage
	flag.Parse()

	if !*enc && !*dec {
		usage()
	}

	xkey := []byte(*key)
	xnonce, err := hex.DecodeString(*nonce)
	ck(err)

	block := newCipher(*cipher_, xkey)
	aead := newAEAD(*mode, block)
	if len(xnonce) != aead.NonceSize() {
		log.Fatalf("expected nonce size %d, got %d", aead.NonceSize(), len(xnonce))
	}

	if flag.NArg() < 1 {
		buf, err := io.ReadAll(os.Stdin)
		ck(err)
		os.Stdout.Write(dump(aead, buf, xnonce))
	} else {
		for _, name := range flag.Args() {
			buf, err := os.ReadFile(name)
			if ek(err) {
				continue
			}
			os.Stdout.Write(dump(aead, buf, xnonce))
		}
	}
	os.Exit(status)
}

func newCipher(cipher_ string, key []byte) cipher.Block {
	var (
		block cipher.Block
		err   error
	)
	switch cipher_ {
	case "aes":
		block, err = aes.NewCipher(key)
	case "des":
		block, err = des.NewCipher(key)
	default:
		log.Fatalf("unknown cipher %q", cipher_)
	}
	ck(err)
	return block
}

func newAEAD(mode string, block cipher.Block) cipher.AEAD {
	var (
		aead cipher.AEAD
		err  error
	)
	switch mode {
	case "gcm":
		aead, err = cipher.NewGCM(block)
	default:
		log.Fatalf("unknown mode %q", aead)
	}
	ck(err)
	return aead
}

func dump(aead cipher.AEAD, buf, nonce []byte) []byte {
	switch {
	case *enc:
		return aead.Seal(nil, nonce, buf, nil)
	case *dec:
		plain, err := aead.Open(nil, nonce, buf, nil)
		if ek(err) {
			return nil
		}
		return plain
	default:
		log.Fatalf("invalid operation, expected encrypt or decryption mode!")
	}
	return nil
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: aead [options] [file] ...")
	flag.PrintDefaults()
	fmt.Fprintf(os.Stderr, "\nciphers: aes des\n")
	fmt.Fprintf(os.Stderr, "modes: gcm\n")
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func ek(err error) bool {
	if err != nil {
		fmt.Fprintln(os.Stderr, "aead:", err)
		status = 1
		return true
	}
	return false
}
