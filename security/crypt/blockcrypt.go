package main

import (
	"bufio"
	"crypto/aes"
	"crypto/cipher"
	"crypto/des"
	"crypto/rand"
	"encoding/hex"
	"flag"
	"fmt"
	"io"
	"log"
	"os"
	"strings"

	"golang.org/x/crypto/blowfish"
	"golang.org/x/crypto/tea"
	"golang.org/x/crypto/twofish"
	"golang.org/x/crypto/xtea"
)

var flags struct {
	alg  string
	mode string
	off  int
}

func main() {
	log.SetFlags(0)
	log.SetPrefix("blockcrypt: ")

	var op string
	flag.StringVar(&flags.alg, "a", "aes256", "algorithm to use")
	flag.StringVar(&flags.mode, "m", "cfb", "mode of operation")
	flag.IntVar(&flags.off, "o", 0, "skip reading first n bytes")
	flag.Bool("g", false, "generate keys")
	flag.Bool("e", false, "encrypt input to output")
	flag.Bool("d", false, "decrypt input to output")
	flag.Usage = usage
	flag.Parse()
	flag.Visit(func(f *flag.Flag) {
		switch f.Name {
		case "e", "d", "g":
			op = f.Name
		}
	})

	switch op {
	case "g":
		genkey()
	case "e":
		encrypt(flag.Arg(0), flag.Arg(1), flag.Arg(2))
	case "d":
		decrypt(flag.Arg(0), flag.Arg(1), flag.Arg(2))
	default:
		usage()
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage:")
	fmt.Fprintln(os.Stderr, "       blockcrypt [options] -g")
	fmt.Fprintln(os.Stderr, "       blockcrypt [options] -e keyfile [input] [output]")
	fmt.Fprintln(os.Stderr, "       blockcrypt [options] -d keyfile [input] [output]")
	flag.PrintDefaults()
	fmt.Fprintln(os.Stderr, "available algorithms: aes128 aes192 aes256 blowfish des 3des tea twofish xtea")
	fmt.Fprintln(os.Stderr, "available modes: ecb cbc cfb ctr ofb")
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func genkey() {
	var (
		key   []byte
		blksz int
	)
	switch flags.alg {
	case "aes128":
		key = genrand("key", 16)
		blksz = aes.BlockSize
	case "aes192":
		key = genrand("key", 24)
		blksz = aes.BlockSize
	case "aes256":
		key = genrand("key", 32)
		blksz = aes.BlockSize
	case "des":
		key = genrand("key", 8)
		blksz = des.BlockSize
	case "3des":
		key = genrand("key", 24)
		blksz = des.BlockSize
	case "blowfish":
		key = genrand("key", 8)
		blksz = blowfish.BlockSize
	case "tea":
		key = genrand("key", 16)
		blksz = tea.BlockSize
	case "twofish":
		key = genrand("key", 16)
		blksz = twofish.BlockSize
	case "xtea":
		key = genrand("key", 16)
		blksz = xtea.BlockSize
	default:
		log.Fatalf("unknown algorithm %q", flags.alg)
	}
	fmt.Printf("alg=%s\n", flags.alg)
	fmt.Printf("mode=%s\n", flags.mode)
	fmt.Printf("key=%s\n", hex.EncodeToString(key))

	var iv []byte
	switch flags.mode {
	case "ecb":
	case "cbc", "cfb", "ctr", "ofb":
		iv = genrand("iv", blksz)
	default:
		log.Fatalf("unknown mode %q", flags.mode)
	}
	if iv != nil {
		fmt.Printf("iv=%s\n", hex.EncodeToString(iv))
	}
}

func readparams(typ int, name string) map[string][]byte {
	f, err := os.Open(name)
	ck(err)

	p := map[string][]byte{
		"alg":  []byte(flags.alg),
		"mode": []byte(flags.mode),
	}
	s := bufio.NewScanner(f)
	for s.Scan() {
		t := strings.Split(s.Text(), "=")
		if len(t) != 2 {
			continue
		}
		for i := range t {
			t[i] = strings.TrimSpace(t[i])
		}

		b, err := hex.DecodeString(t[1])
		if err != nil {
			b = []byte(t[1])
		}

		p[t[0]] = b
	}

	check := func(names ...string) {
		tab := [...]string{
			'k': "key",
		}
		for _, name := range names {
			if p[name] == nil {
				log.Fatalf("%s file missing parameter %q", tab[typ], name)
			}
		}
	}

	switch typ {
	case 'k':
		check("key")
		if flags.mode != "ecb" {
			check("iv")
		}
	}

	return p
}

func startcrypt(typ int, keyfile, input, output string) (r *bufio.Reader, w *bufio.Writer, b cipher.Block, bm cipher.BlockMode, sm cipher.Stream) {
	var err error
	ifd, ofd := os.Stdin, os.Stdout
	if input != "" {
		ifd, err = os.Open(input)
		ck(err)
	}
	if output != "" {
		ofd, err = os.Create(output)
		ck(err)
	}

	r = bufio.NewReader(ifd)
	w = bufio.NewWriter(ofd)
	p := readparams('k', keyfile)

	key := p["key"]
	switch string(p["alg"]) {
	case "aes128", "aes192", "aes256":
		b, err = aes.NewCipher(key)
	case "des":
		b, err = des.NewCipher(key)
	case "3des":
		b, err = des.NewTripleDESCipher(key)
	case "blowfish":
		b, err = blowfish.NewCipher(key)
	case "tea":
		b, err = tea.NewCipher(key)
	case "twofish":
		b, err = twofish.NewCipher(key)
	case "xtea":
		b, err = xtea.NewCipher(key)
	default:
		log.Fatalf("unknown algorithm %q", p["alg"])
	}
	ck(err)

	iv := p["iv"]
	switch string(p["mode"]) {
	case "ecb":
	case "cbc":
		if typ == 'e' {
			bm = cipher.NewCBCEncrypter(b, iv)
		} else {
			bm = cipher.NewCBCDecrypter(b, iv)
		}
	case "cfb":
		if typ == 'e' {
			sm = cipher.NewCFBEncrypter(b, iv)
		} else {
			sm = cipher.NewCFBDecrypter(b, iv)
		}
	case "ctr":
		sm = cipher.NewCTR(b, iv)
	case "ofb":
		sm = cipher.NewOFB(b, iv)
	default:
		log.Fatalf("unknown mode %q", p["mode"])
	}

	if flags.off > 0 {
		io.ReadAtLeast(r, make([]byte, flags.off), flags.off)
	}

	return
}

func encrypt(keyfile, input, output string) {
	r, w, b, bm, sm := startcrypt('e', keyfile, input, output)
	var err error
	switch {
	case sm != nil:
		sw := cipher.StreamWriter{S: sm, W: w}
		_, err = io.Copy(sw, r)
	case bm != nil:
		err = blockcrypt(bm.BlockSize(), r, w, bm.CryptBlocks)
	default:
		err = blockcrypt(b.BlockSize(), r, w, b.Encrypt)
	}
	ck(err)
	ck(w.Flush())
}

func decrypt(keyfile, input, output string) {
	r, w, b, bm, sm := startcrypt('d', keyfile, input, output)
	var err error
	switch {
	case sm != nil:
		sr := cipher.StreamReader{S: sm, R: r}
		_, err = io.Copy(w, sr)
	case bm != nil:
		err = blockcrypt(bm.BlockSize(), r, w, bm.CryptBlocks)
	default:
		err = blockcrypt(b.BlockSize(), r, w, b.Decrypt)
	}
	ck(err)
	ck(w.Flush())
}

func genrand(name string, size int) []byte {
	buf := make([]byte, size)
	_, err := rand.Read(buf)
	if err != nil {
		log.Fatalf("failed to generate random data for %s: %s", name, err)
	}
	return buf
}

func blockcrypt(size int, r io.Reader, w io.Writer, f func(dst, src []byte)) error {
	src := make([]byte, size)
	dst := make([]byte, size)
	for {
		for i := range src {
			src[i] = 0
		}
		n, err := r.Read(src)
		if n > 0 {
			f(dst, src)
			w.Write(dst)
		}
		if err == io.EOF {
			break
		}
		if err != nil {
			return err
		}
	}
	return nil
}
