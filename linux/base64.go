package main

import (
	"bytes"
	"encoding/base64"
	"flag"
	"fmt"
	"io"
	"os"
)

var (
	dec  = flag.Bool("d", false, "decode data")
	wrap = flag.Int64("w", 76, "wrap encoded lines after cols character")
)

func main() {
	flag.Usage = usage
	flag.Parse()

	codec := encode
	if *dec {
		codec = decode
	}

	switch flag.NArg() {
	case 0:
		codec(os.Stdin)
	case 1:
		f, err := os.Open(flag.Arg(0))
		ck(err)
		defer f.Close()
		ck(codec(f))
	default:
		usage()
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] file")
	os.Exit(1)
}

func ck(err error) {
	if err != nil {
		fmt.Fprintln(os.Stderr, "base64:", err)
		os.Exit(1)
	}
}

func decode(r io.Reader) error {
	_, err := io.Copy(os.Stdout, base64.NewDecoder(base64.StdEncoding, r))
	return err
}

func encode(r io.Reader) error {
	p := new(bytes.Buffer)
	defer flush(p)

	e := base64.NewEncoder(base64.StdEncoding, p)
	defer e.Close()

	var b [8192]byte
	for {
		n, err := r.Read(b[:])
		if n > 0 {
			e.Write(b[:n])
		}
		write(p)

		if err == io.EOF {
			break
		}
		if err != nil {
			return err
		}
	}

	return nil
}

func write(p *bytes.Buffer) {
	if *wrap <= 0 {
		io.Copy(os.Stdout, p)
	} else {
		for int64(p.Len()) >= *wrap {
			n, _ := io.CopyN(os.Stdout, p, *wrap)
			if n == 0 {
				break
			}
			fmt.Println()
		}
	}
}

func flush(p *bytes.Buffer) {
	if p.Len() > 0 {
		io.Copy(os.Stdout, p)
	}
	if *wrap > 0 {
		fmt.Println()
	}
}
